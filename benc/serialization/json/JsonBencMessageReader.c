/* vim: set expandtab ts=4 sw=4: */
/*
 * You may redistribute this program and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "benc/serialization/json/JsonBencMessageReader.h"
#include "benc/List.h"
#include "benc/Dict.h"
#include "benc/String.h"
#include "exception/Er.h"
#include "memory/Allocator.h"
#include "wire/Message.h"
#include "util/Gcc.h"
#include "util/Hex.h"
#include "util/Base10.h"

#include <stdbool.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * The Er system in this tree is fatal by design: Er_raise prints the message
 * and aborts the process. That is fine for the daemon, but the JSON reader is
 * required to tolerate malformed input and report a pleasant error instead
 * (JsonBencMessageReader_readNoExcept, cjdroute2 config loading and the fuzz
 * harness all rely on errors being recoverable). Re-establish the original
 * unwinding behavior for this translation unit only: Er_raise unwinds to the
 * nearest Er_check boundary via a longjmp.
 */
struct Er_Bjb_ {
    jmp_buf jb;
    struct Er_Ret* err;
};
static __thread struct Er_Bjb_* Er__activeBjb_ = NULL;

static struct Er_Ret* Er__raiseLocal(
    char* file, int line, struct Allocator* alloc, char* format, ...)
{
    va_list args;
    va_start(args, format);
    if (!alloc) {
        fprintf(stderr, "%s:%d ", file, line);
        vfprintf(stderr, format, args);
        fprintf(stderr, "\n");
        abort();
    }
    int written = snprintf(NULL, 0, "%s:%d ", file, line);
    Assert_true(written >= 0);
    va_list argsCopy;
    va_copy(argsCopy, args);
    int written2 = vsnprintf(NULL, 0, format, argsCopy);
    Assert_true(written2 >= 0);
    va_end(argsCopy);
    int len = written + written2 + 1;
    char* buf = Allocator_calloc(alloc, len, 1);
    snprintf(buf, len, "%s:%d ", file, line);
    vsnprintf(&buf[written], len - written, format, args);
    struct Er_Ret* res = Allocator_calloc(alloc, sizeof(struct Er_Ret), 1);
    res->message = buf;
    va_end(args);
    return res;
}

#undef Er_raise
#define Er_raise(alloc, ...) \
    do { \
        struct Er_Ret* Er_ret = Er__raiseLocal(Gcc_SHORT_FILE, Gcc_LINE, (alloc), __VA_ARGS__); \
        if (Er_ret && Er__activeBjb_) { \
            Er__activeBjb_->err = Er_ret; \
            longjmp(Er__activeBjb_->jb, 1); \
        } \
        Er__assertFail(Er_ret); \
    } while (0)

#undef Er_check
#define Er_check(ret, expr) \
    __extension__ ({ \
        struct Er_Bjb_ Er_bj_ = { .err = NULL }; \
        struct Er_Bjb_* Er_prevBjb_ = Er__activeBjb_; \
        volatile typeof(expr) Er_res_ = (typeof(expr))0; \
        volatile bool Er_jumped_ = false; \
        Er__activeBjb_ = &Er_bj_; \
        if (setjmp(Er_bj_.jb) == 0) { \
            Er_res_ = (expr); \
        } else { \
            Er_jumped_ = true; \
        } \
        Er__activeBjb_ = Er_prevBjb_; \
        *(ret) = Er_jumped_ ? Er_bj_.err : NULL; \
        Er_res_; \
    })

struct Context {
    struct Message* const msg;
    struct Allocator* const alloc;
    const bool lax;
    int line;
    uintptr_t beginningLastLine;
};

static int getColumn(struct Context* ctx)
{
    return (uintptr_t) ctx->msg->bytes - ctx->beginningLastLine;
}

#define ERROR0(ctx, message) \
    Er_raise(ctx->alloc,    \
        "Error parsing config (line %d column %d): " message, \
        ctx->line, getColumn(ctx))
#define ERROR(ctx, message, ...) \
    Er_raise(ctx->alloc,    \
        "Error parsing config (line %d column %d): " message, \
        ctx->line, getColumn(ctx), __VA_ARGS__)

static Er_DEFUN(uint8_t peak(struct Context* ctx))
{
    if (!ctx->msg->length) { ERROR0(ctx, "Out of content while reading"); }
    Er_ret(ctx->msg->bytes[0]);
}

static Er_DEFUN(void skip(struct Context* ctx, int num))
{
    if (num > ctx->msg->length) { ERROR0(ctx, "Out of content while reading"); }
    for (int i = 0; i < num; i++) {
        if (ctx->msg->bytes[i] == '\n') {
            ctx->beginningLastLine = (uintptr_t) &ctx->msg->bytes[i];
            ctx->line++;
        }
    }
    Er(Message_eshift(ctx->msg, -num));
    Er_ret();
}

static Er_DEFUN(bool assertChar(struct Context* ctx, char chr, bool lax))
{
    char c = Er(peak(ctx));
    if (c != chr) {
        if (lax == true) { Er_ret(false); }
        ERROR(ctx, "Expected char [%c] but got [%c]", chr, c);
    }
    Er_ret(true);
}

static Er_DEFUN(void parseComment(struct Context* ctx))
{
    Er(assertChar(ctx, '/', false));
    Er(skip(ctx, 1));
    uint8_t secondChar = Er(peak(ctx));
    if (secondChar != '/' && secondChar != '*') { ERROR(ctx, "Unexpected char [%c]", secondChar); }
    bool lastCharSplat = false;
    for (;;) {
        Er(skip(ctx, 1));
        uint8_t chr = Er(peak(ctx));
        if (lastCharSplat && secondChar == '*' && chr == '/') {
            // get rid of the trailing *
            Er(skip(ctx, 1));
        } else if (secondChar == '/' && chr == '\n') {
        } else {
            lastCharSplat = (chr == '*');
            continue;
        }
        Er_ret();
    }
}

static Er_DEFUN(void parseWhitespaceAndComments(struct Context* ctx))
{
    for (;;) {
        switch (Er(peak(ctx))) {
            case '\n':
            case ' ':
            case '\r':
            case '\t':
                Er(skip(ctx, 1));
                continue;

            case '/':
                Er(parseComment(ctx));
                continue;

            default: break;
        }
        Er_ret();
    }
    ERROR0(ctx, "Reached end of message while parsing");
}

static Er_DEFUN(String* parseString(struct Context* ctx))
{
    Er(assertChar(ctx, '"', false));
    int line = ctx->line;
    uintptr_t beginningLastLine = ctx->beginningLastLine;
    int msgLen = ctx->msg->length;

    String* out = NULL;
    uint32_t pos = 0;
    #define PUSHOUT(chr) do { \
        if (out) {                              \
            Assert_true(out->len > pos);        \
            out->bytes[pos] = (chr);            \
        }                                       \
        pos++;                                  \
    } while (0)
    // CHECKFILES_IGNORE expecting a ;

    for (;;) {
        Er(skip(ctx, 1));
        uint8_t bchar = Er(peak(ctx));
        switch (bchar) {
            case '"': {
                Er(skip(ctx, 1));
                if (out) { Er_ret(out); }
                // got the length, reset and then copy the string next cycle
                ctx->line = line;
                ctx->beginningLastLine = beginningLastLine;
                Er(Message_eshift(ctx->msg, msgLen - ctx->msg->length));
                out = String_newBinary(NULL, pos, ctx->alloc);
                pos = 0;
                continue;
            }
            case '\0':
            case '\n': {
                ERROR0(ctx, "Unterminated string");
            }
            case '\\': {
                Er(skip(ctx, 1));
                uint8_t x = Er(peak(ctx));
                Er(skip(ctx, 1));
                if (x != 'x') {
                    ERROR0(ctx, "Char \\ only allowed if followed by x (as in \\xff)");
                }
                uint8_t high = Er(peak(ctx));
                Er(skip(ctx, 1));
                uint8_t low = Er(peak(ctx));
                int byte = Hex_decodeByte(high, low);
                if (byte < 0 || (byte > 255)) { ERROR0(ctx, "Invalid hex encoding"); }
                PUSHOUT(byte);
                continue;
            }
            default: {
                PUSHOUT(bchar);
                continue;
            }
        }
    }
    #undef PUSHOUT
}

static Er_DEFUN(int64_t parseInteger(struct Context* ctx))
{
    Er_ret( Er(Base10_read(ctx->msg)) );
}

static Er_DEFUN(Object* parseGeneric(struct Context* ctx));

static Er_DEFUN(List* parseList(struct Context* ctx))
{
    Er(assertChar(ctx, '[', false));
    Er(skip(ctx, 1));
    struct List_Item* first = NULL;
    struct List_Item* last = NULL;
    for (int i = 0; ; i++) {
        for (;;) {
            Er(parseWhitespaceAndComments(ctx));
            // lax mode, allow ,, extra ,,, commas
            if (!ctx->lax || Er(peak(ctx)) != ',') { break; }
            Er(skip(ctx, 1));
        }
        if (Er(peak(ctx)) == ']') {
            Er(skip(ctx, 1));
            List* out = Allocator_malloc(ctx->alloc, sizeof(List));
            *out = first;
            Er_ret(out);
        }
        if (i && Er(assertChar(ctx, ',', ctx->lax))) {
            Er(skip(ctx, 1));
            Er(parseWhitespaceAndComments(ctx));
        }
        struct List_Item* item = Allocator_calloc(ctx->alloc, sizeof(struct List_Item), 1);
        item->elem = Er(parseGeneric(ctx));
        if (last) {
            last->next = item;
        } else {
            first = item;
        }
        last = item;
    }
}

static Er_DEFUN(Dict* parseDict(struct Context* ctx))
{
    Er(assertChar(ctx, '{', false));
    Er(skip(ctx, 1));
    struct Dict_Entry* last = NULL;
    struct Dict_Entry* first = NULL;
    for (int i = 0; ; i++) {
        for (;;) {
            Er(parseWhitespaceAndComments(ctx));
            if (!ctx->lax || Er(peak(ctx)) != ',') { break; }
            Er(skip(ctx, 1));
        }
        if (Er(peak(ctx)) == '}') {
            Er(skip(ctx, 1));
            Dict* out = Allocator_malloc(ctx->alloc, sizeof(Dict));
            *out = first;
            Er_ret(out);
        }
        if (i && Er(assertChar(ctx, ',', ctx->lax))) {
            Er(skip(ctx, 1));
            Er(parseWhitespaceAndComments(ctx));
        }
        struct Dict_Entry* entry = Allocator_calloc(ctx->alloc, sizeof(struct Dict_Entry), 1);
        entry->key = Er(parseString(ctx));
        Er(parseWhitespaceAndComments(ctx));
        if (Er(assertChar(ctx, ':', ctx->lax))) {
            Er(skip(ctx, 1));
            Er(parseWhitespaceAndComments(ctx));
        }
        entry->val = Er(parseGeneric(ctx));
        if (last) {
            last->next = entry;
        } else {
            first = entry;
        }
        last = entry;
    }
}

static Er_DEFUN(Object* parseGeneric(struct Context* ctx))
{
    Object* out = Allocator_calloc(ctx->alloc, sizeof(Object), 1);
    uint8_t c = Er(peak(ctx));
    switch (c) {
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            out->type = Object_INTEGER;
            out->as.number = Er(parseInteger(ctx));
            break;
        }
        case '[': {
            out->type = Object_LIST;
            out->as.list = Er(parseList(ctx));
            break;
        }
        case '{': {
            out->type = Object_DICT;
            out->as.dictionary = Er(parseDict(ctx));
            break;
        }
        case '"': {
            out->type = Object_STRING;
            out->as.string = Er(parseString(ctx));
            break;
        }
        default:
            ERROR(ctx, "While looking for something to parse: "
                   "expected one of - 0 1 2 3 4 5 6 7 8 9 [ { \", found [%c]", c);
    }
    Er_ret(out);
}

Er_DEFUN(Dict* JsonBencMessageReader_read(
    struct Message* msg,
    struct Allocator* alloc,
    bool lax
)) {
    struct Context ctx = {
        .msg = msg,
        .alloc = alloc,
        .lax = lax,
        .line = 1,
        .beginningLastLine = (uintptr_t) msg->bytes
    };
    Er_ret( Er(parseDict(&ctx)) );
}

const char* JsonBencMessageReader_readNoExcept(
    struct Message* msg,
    struct Allocator* alloc,
    Dict** outPtr,
    bool lax)
{
    struct Er_Ret* er = NULL;
    Dict* out = Er_check(&er, JsonBencMessageReader_read(msg, alloc, lax));
    if (er) {
        return er->message;
    }
    *outPtr = out;
    return NULL;
}

