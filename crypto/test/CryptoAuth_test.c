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
#include "crypto/random/Random.h"
#include "crypto/CryptoAuth.h"
#include "benc/String.h"
#include "memory/MallocAllocator.h"
#include "util/events/EventBase.h"
#include "util/Assert.h"
#include "util/Bits.h"
#include "util/Hex.h"
#include "util/Endian.h"
#include "util/log/FileWriterLog.h"
#include "wire/CryptoHeader.h"

#define PRIVATEKEY_A \
    ((uint8_t[32]) { 0x53,0xff,0x22,0xb2,0xeb,0x94,0xce,0x8c,0x5f,0x18,0x52,0xc0,0xf5,0x57,0xeb,0x90,0x1f,0x06,0x7e,0x52,0x73,0xd5,0x41,0xe0,0xa2,0x1e,0x14,0x3c,0x20,0xdf,0xf9,0xda })
#define PUBLICKEY_A \
    ((uint8_t[32]) { 0xe3,0xff,0x75,0xaf,0x6e,0x44,0x14,0x49,0x4d,0xf2,0x2f,0x20,0x0f,0xfe,0xaa,0x56,0xe7,0x97,0x6d,0x99,0x1d,0x33,0xcc,0x87,0xf5,0x24,0x27,0xe2,0x7f,0x83,0x23,0x5d })

#define PRIVATEKEY_B \
    ((uint8_t[32]) { 0xb7,0x1c,0x4f,0x43,0xe3,0xd4,0xb1,0x87,0x9b,0x50,0x65,0xd4,0x4a,0x1c,0xb4,0x3e,0xaf,0x07,0xdd,0xba,0x96,0xde,0x6a,0x72,0xca,0x76,0x1c,0x4e,0xf4,0xbd,0x29,0x88 })
#define PUBLICKEY_B \
    ((uint8_t[32]) { 0x27,0xc3,0x03,0xcd,0xc1,0xf9,0x6e,0x4b,0x28,0xd5,0x1c,0x75,0x13,0x0a,0xff,0x6c,0xad,0x52,0x09,0x8f,0x2d,0x75,0x26,0x15,0xb7,0xb6,0x50,0x9e,0xd6,0xa8,0x94,0x77 })

#define USEROBJ "This represents a user"

struct Context
{
    struct CryptoAuth* ca1;
    struct CryptoAuth_Session* sess1;

    struct CryptoAuth* ca2;
    struct CryptoAuth_Session* sess2;

    struct Allocator* alloc;
    struct Log* log;
    struct Random* rand;
    struct EventBase* base;
};

static struct Context* init(uint8_t* privateKeyA,
                            uint8_t* publicKeyA,
                            uint8_t* password,
                            uint8_t* privateKeyB,
                            uint8_t* publicKeyB)
{
    struct Allocator* alloc = MallocAllocator_new(1048576);
    struct Context* ctx = Allocator_calloc(alloc, sizeof(struct Context), 1);
    ctx->alloc = alloc;
    struct Log* logger = ctx->log = FileWriterLog_new(stdout, alloc);
    struct Random* rand = ctx->rand = Random_new(alloc, logger, NULL);
    struct EventBase* base = ctx->base = EventBase_new(alloc);

    ctx->ca1 = CryptoAuth_new(alloc, privateKeyA, base, logger, rand);
    ctx->sess1 = CryptoAuth_newSession(ctx->ca1, alloc, publicKeyB, false, "cif1");

    ctx->ca2 = CryptoAuth_new(alloc, privateKeyB, base, logger, rand);
    if (password) {
        String* passStr = String_CONST(password);
        CryptoAuth_setAuth(passStr, NULL, ctx->sess1);
        CryptoAuth_addUser(passStr, String_new(USEROBJ, alloc), ctx->ca2);
    }
    ctx->sess2 = CryptoAuth_newSession(ctx->ca2, alloc, publicKeyA, false, "cif2");

    return ctx;
}

static struct Context* simpleInit()
{
    return init(PRIVATEKEY_A, PUBLICKEY_A, NULL, PRIVATEKEY_B, PUBLICKEY_B);
}

static struct Message* encryptMsg(struct Context* ctx,
                                  struct CryptoAuth_Session* encryptWith,
                                  const char* x)
{
    struct Allocator* alloc = Allocator_child(ctx->alloc);
    int len = (((CString_strlen(x)+1) / 8) + 1) * 8;
    struct Message* msg = Message_new(len, CryptoHeader_SIZE, alloc);
    CString_strcpy(msg->bytes, x);
    msg->length = CString_strlen(x);
    msg->bytes[msg->length] = 0;
    Assert_true(!CryptoAuth_encrypt(encryptWith, msg));
    Assert_true(msg->length > ((int)CString_strlen(x) + 4));
    return msg;
}

static void decryptMsg(struct Context* ctx,
                       struct Message* msg,
                       struct CryptoAuth_Session* decryptWith,
                       const char* x)
{
    if (!x) {
        // x is null implying it is expected to fail.
        Assert_true(CryptoAuth_decrypt(decryptWith, msg));
    } else {
        Assert_true(!CryptoAuth_decrypt(decryptWith, msg));
        if ((int)CString_strlen(x) != msg->length ||
            CString_strncmp(msg->bytes, x, msg->length))
        {
            Assert_failure("expected [%s](%d), got [%s](%d)\n",
                x, (int)CString_strlen(x), msg->bytes, msg->length);
        }
    }
}

static void sendToIf1(struct Context* ctx, const char* x)
{
    struct Message* msg = encryptMsg(ctx, ctx->sess2, x);
    decryptMsg(ctx, msg, ctx->sess1, x);
    Allocator_free(msg->alloc);
}

static void sendToIf2(struct Context* ctx, const char* x)
{
    struct Message* msg = encryptMsg(ctx, ctx->sess1, x);
    decryptMsg(ctx, msg, ctx->sess2, x);
    Allocator_free(msg->alloc);
}

static void normal()
{
    struct Context* ctx = simpleInit();
    sendToIf2(ctx, "hello world");
    sendToIf1(ctx, "hello cjdns");
    sendToIf2(ctx, "hai");
    sendToIf1(ctx, "goodbye");
    Allocator_free(ctx->alloc);
}

static void repeatKey()
{
    struct Context* ctx = simpleInit();
    sendToIf2(ctx, "hello world");
    sendToIf2(ctx, "r u thar?");
    sendToIf1(ctx, "hello cjdns");
    sendToIf2(ctx, "hai");
    sendToIf1(ctx, "goodbye");
    Allocator_free(ctx->alloc);
}

static void repeatHello()
{
    struct Context* ctx = simpleInit();
    sendToIf2(ctx, "hello world");
    sendToIf2(ctx, "r u thar?");
    sendToIf1(ctx, "hello cjdns");
    sendToIf2(ctx, "hai");
    sendToIf1(ctx, "goodbye");
    Allocator_free(ctx->alloc);
}

static void chatter()
{
    struct Context* ctx = simpleInit();
    sendToIf2(ctx, "hello world");
    sendToIf1(ctx, "hello cjdns");
    sendToIf2(ctx, "hai");
    sendToIf1(ctx, "goodbye");
    sendToIf1(ctx, "hello cjdns");
    sendToIf2(ctx, "hai");
    sendToIf1(ctx, "goodbye");
    sendToIf1(ctx, "hello cjdns");
    sendToIf2(ctx, "hai");
    sendToIf1(ctx, "goodbye");
    sendToIf1(ctx, "hello cjdns");
    sendToIf2(ctx, "hai");
    sendToIf1(ctx, "goodbye");
    Allocator_free(ctx->alloc);
}

static void auth()
{
    struct Context* ctx = init(PRIVATEKEY_A, PUBLICKEY_A, "password", PRIVATEKEY_B, PUBLICKEY_B);
    sendToIf2(ctx, "hello world");
    sendToIf1(ctx, "hello cjdns");
    sendToIf2(ctx, "hai");
    sendToIf1(ctx, "goodbye");
    Allocator_free(ctx->alloc);
}

static void replayKeyPacket(int scenario)
{
    struct Context* ctx = simpleInit();

    sendToIf2(ctx, "hello world");

    struct Message* msg = encryptMsg(ctx, ctx->sess2, "hello replay key");
    struct Message* toReplay = Message_clone(msg, ctx->alloc);
    decryptMsg(ctx, msg, ctx->sess1, "hello replay key");

    if (scenario == 1) {
        // the packet is failed because we know it's a dupe from the temp key.
        decryptMsg(ctx, toReplay, ctx->sess1, NULL);
    }

    sendToIf2(ctx, "first traffic packet");

    if (scenario == 2) {
        decryptMsg(ctx, toReplay, ctx->sess1, NULL);
    }

    sendToIf1(ctx, "second traffic packet");

    if (scenario == 3) {
        // If we replay at this stage, the packet is dropped as a stray key
        decryptMsg(ctx, toReplay, ctx->sess1, NULL);
    }

    Allocator_free(ctx->alloc);
}

/**
 * Alice and Bob both decided they wanted to talk to eachother at precisely the same time.
 * This means two Hello packets crossed on the wire. Both arrived at their destination but
 * if each triggers a re-initialization of the CA session, nobody will be synchronized!
 */
static void hellosCrossedOnTheWire()
{
    struct Context* ctx = simpleInit();
    Bits_memcpy(ctx->sess2->herPublicKey, ctx->ca1->publicKey, 32);

    struct Message* hello2 = encryptMsg(ctx, ctx->sess2, "hello2");
    struct Message* hello1 = encryptMsg(ctx, ctx->sess1, "hello1");

    decryptMsg(ctx, hello2, ctx->sess1, "hello2");
    decryptMsg(ctx, hello1, ctx->sess2, "hello1");

    sendToIf2(ctx, "hello world");
    sendToIf1(ctx, "hello cjdns");
    sendToIf2(ctx, "hai");
    sendToIf1(ctx, "goodbye");

    Allocator_free(ctx->alloc);
}

static void reset()
{
    struct Context* ctx = simpleInit();
    sendToIf2(ctx, "hello world");
    sendToIf1(ctx, "hello cjdns");
    sendToIf2(ctx, "hai");
    sendToIf1(ctx, "brb");

    Assert_true(CryptoAuth_getState(ctx->sess1) == CryptoAuth_State_ESTABLISHED);
    Assert_true(CryptoAuth_getState(ctx->sess2) == CryptoAuth_State_ESTABLISHED);

    CryptoAuth_reset(ctx->sess1);

    // sess2 still talking to sess1 but sess1 is reset and cannot read the packets.
    decryptMsg(ctx, encryptMsg(ctx, ctx->sess2, "will be lost"), ctx->sess1, NULL);
    decryptMsg(ctx, encryptMsg(ctx, ctx->sess2, "lost"), ctx->sess1, NULL);

    // This is because we want to prevent replay attacks from tearing down a session.
    decryptMsg(ctx, encryptMsg(ctx, ctx->sess1, "hello"), ctx->sess2, "hello");

    sendToIf1(ctx, "hello again");
    sendToIf2(ctx, "hai");
    sendToIf1(ctx, "ok works");
    sendToIf2(ctx, "yup");

    Assert_true(CryptoAuth_getState(ctx->sess1) == CryptoAuth_State_ESTABLISHED);
    Assert_true(CryptoAuth_getState(ctx->sess2) == CryptoAuth_State_ESTABLISHED);

    Allocator_free(ctx->alloc);
}

// This is slightly different from replayKeyPacket because the second key packet is valid,
// it's just delayed.
static void twoKeyPackets(int scenario)
{
    struct Context* ctx = simpleInit();

    sendToIf2(ctx, "hello world");
    sendToIf1(ctx, "key packet 1");
    struct Message* key2 = encryptMsg(ctx, ctx->sess2, "key packet 2");

    if (scenario == 1) {
        sendToIf1(ctx, "key packet 3");
        decryptMsg(ctx, key2, ctx->sess1, "key packet 2");
    } else if (scenario == 2) {
        sendToIf2(ctx, "initial data packet");
        decryptMsg(ctx, key2, ctx->sess1, "key packet 2");
        sendToIf1(ctx, "second data packet");
        sendToIf2(ctx, "third data packet");
    } else if (scenario == 3) {
        sendToIf2(ctx, "initial data packet");
        sendToIf1(ctx, "second data packet");
        decryptMsg(ctx, key2, ctx->sess1, NULL);
    }
    Allocator_free(ctx->alloc);
}

int main()
{
    normal();
    repeatKey();
    repeatHello();
    chatter();
    auth();
    replayKeyPacket(1);
    replayKeyPacket(2);
    replayKeyPacket(3);
    hellosCrossedOnTheWire();
    reset();
    twoKeyPackets(1);
    twoKeyPackets(2);
    twoKeyPackets(3);
    return 0;
}
