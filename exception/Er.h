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
#ifndef Er_H
#define Er_H

#include "memory/Allocator.h"
#include "util/Gcc.h"

#include "util/Linker.h"
Linker_require("exception/Er.c")

struct Er_Ret
{
    const char* message;
};

Gcc_PRINTF(4, 5)
Gcc_USE_RET
struct Er_Ret* Er__raise(char* file, int line, struct Allocator* alloc, char* format, ...);

/*
 * The Er exception system was removed along with the JavaScript preprocessor.
 * Functions declared with Er_DEFUN are now ordinary C functions. Any error they
 * encounter is fatal: Er_raise prints the message and aborts the process, and
 * because a failing callee never returns, the "winding" macros collapse to
 * plain evaluation of their expression:
 *
 *   Er(expr)        -> (expr)         the callee either succeeds or aborts
 *   Er_assert(expr) -> (void)(expr)   same, but discard any returned value
 *   Er_check(r,e)   -> (e)            error recovery is no longer available
 *   Er_raise(...)   -> fail (abort)   no unwinding to a caller
 *   Er_ret(v)       -> return (v)     normal function return
 */
#define Er_DEFUN(...) __VA_ARGS__

Gcc_NORETURN
void Er__assertFail(struct Er_Ret* er);
#define Er_raise(...) \
    do { \
        Er__assertFail(Er__raise(Gcc_SHORT_FILE, Gcc_LINE, __VA_ARGS__)); \
    } while (0)

#define Er(expr) (expr)

#define Er_assert(expr) (expr)

#define Er_check(ret, expr) (expr)

#define Er_ret(...) Er_ret_PASTE(Er_ret_apply, Er_ret_NARGS(__VA_ARGS__))(__VA_ARGS__)
#define Er_ret_PASTE(a, b) Er_ret_PASTE_I(a, b)
#define Er_ret_PASTE_I(a, b) a##b
#define Er_ret_NARGS(...) Er_ret_NARGS_SEL(, ## __VA_ARGS__, 1, 0)
#define Er_ret_NARGS_SEL(_1, _2, N, ...) N
#define Er_ret_apply0() return ;
#define Er_ret_apply1(v) return (v);

static inline struct Er_Ret* Er_unwind(const char* file, int line, struct Er_Ret* ret)
{
    return ret;
}

#endif
