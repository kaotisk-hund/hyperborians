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
#include "crypto/random/test/DeterminentRandomSeed.h"
#include "util/Assert.h"
#include "util/events/Time.h"
#include "util/events/EventBase.h"
#include "util/CString.h"
#include "memory/MallocAllocator.h"
#include "wire/Message.h"
#include "test/FuzzTest.h"
#include "util/Bits.h"
#include "util/CString.h"
#include "util/Hex.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef SUBNODE
    #define testhyperboriaRoute_SUBNODE 1
#else
    #define testhyperboriaRoute_SUBNODE 0
#endif

typedef int (* Test)(int argc, char** argv);
typedef void* (* FuzzTestInit)(struct Allocator* alloc, struct Random* rand);
typedef void (* FuzzTest)(void* ctx, struct Message* fuzz);
typedef struct FuzzTest* (* MkFuzz)(struct Allocator* alloc);

int Random_test_main(int argc, char** argv);
int CryptoAuth_test_main(int argc, char** argv);
int CryptoAuth_unit_test_main(int argc, char** argv);
int ReplayProtector_test_main(int argc, char** argv);
int Sign_test_main(int argc, char** argv);
int NodeStore_test_main(int argc, char** argv);
int VersionList_test_main(int argc, char** argv);
int DHTModules_handleIncoming_test_main(int argc, char** argv);
int DHTModules_handleOutgoing_test_main(int argc, char** argv);
int BSDMessageTypeWrapper_test_main(int argc, char** argv);
int TAPWrapper_root_test_main(int argc, char** argv);
int TUNInterface_ipv4_root_test_main(int argc, char** argv);
int TUNInterface_ipv6_root_test_main(int argc, char** argv);
int TUNInterface_ipv6_withroute_root_test_main(int argc, char** argv);
int TAPDevice_root_test_main(int argc, char** argv);
int TAPInterface_root_test_main(int argc, char** argv);
int FileReader_test_main(int argc, char** argv);
int Allocator_test_main(int argc, char** argv);
int EncodingScheme_test_main(int argc, char** argv);
int LabelSplicer_test_main(int argc, char** argv);
int NumberCompress_test_main(int argc, char** argv);
int Beacon_test_main(int argc, char** argv);
int CryptoAddress_test_main(int argc, char** argv);
int printIp_test_main(int argc, char** argv);
int IpTunnel_test_main(int argc, char** argv);
int RouteGen_test_main(int argc, char** argv);
int Sockaddr_test_main(int argc, char** argv);
int AddrTools_test_main(int argc, char** argv);
int AverageRoller_test_main(int argc, char** argv);
int Base10_test_main(int argc, char** argv);
int Base32_test_main(int argc, char** argv);
int Bits_test_main(int argc, char** argv);
int Checksum_test_main(int argc, char** argv);
int Endian_test_main(int argc, char** argv);
int Hex_test_main(int argc, char** argv);
int Identity_test_main(int argc, char** argv);
int Map_test_main(int argc, char** argv);
int Process_test_main(int argc, char** argv);
int QSort_test_main(int argc, char** argv);
int Seccomp_test_main(int argc, char** argv);
int Set_test_main(int argc, char** argv);
int VarInt_test_main(int argc, char** argv);
void* JsonBencMessageReader_init(struct Allocator* alloc, struct Random* rand);
void JsonBencMessageReader_fuzz(void* vctx, struct Message* fuzz);
void* CryptoAuth_init(struct Allocator* alloc, struct Random* rand);
void CryptoAuth_fuzz(void* vctx, struct Message* fuzz);
void* FramingIface_init(struct Allocator* alloc, struct Random* rand);
void FramingIface_fuzz(void* vctx, struct Message* fuzz);
void* Main_init(struct Allocator* alloc, struct Random* rand);
void Main_fuzz(void* vctx, struct Message* fuzz);
void* Map_init(struct Allocator* alloc, struct Random* rand);
void Map_fuzz(void* vctx, struct Message* fuzz);

static const struct {
    Test func;
    char* name;
} TESTS[] = {
    { Random_test_main, "Random_test" },
    { CryptoAuth_test_main, "CryptoAuth_test" },
    { CryptoAuth_unit_test_main, "CryptoAuth_unit_test" },
    { ReplayProtector_test_main, "ReplayProtector_test" },
    { Sign_test_main, "Sign_test" },
    { NodeStore_test_main, "NodeStore_test" },
    { VersionList_test_main, "VersionList_test" },
    { DHTModules_handleIncoming_test_main, "DHTModules_handleIncoming_test" },
    { DHTModules_handleOutgoing_test_main, "DHTModules_handleOutgoing_test" },
    { BSDMessageTypeWrapper_test_main, "BSDMessageTypeWrapper_test" },
    { TAPWrapper_root_test_main, "TAPWrapper_root_test" },
    { TUNInterface_ipv4_root_test_main, "TUNInterface_ipv4_root_test" },
    { TUNInterface_ipv6_root_test_main, "TUNInterface_ipv6_root_test" },
    { TUNInterface_ipv6_withroute_root_test_main, "TUNInterface_ipv6_withroute_root_test" },
    { TAPDevice_root_test_main, "TAPDevice_root_test" },
    { TAPInterface_root_test_main, "TAPInterface_root_test" },
    { FileReader_test_main, "FileReader_test" },
    { Allocator_test_main, "Allocator_test" },
    { EncodingScheme_test_main, "EncodingScheme_test" },
    { LabelSplicer_test_main, "LabelSplicer_test" },
    { NumberCompress_test_main, "NumberCompress_test" },
    { Beacon_test_main, "Beacon_test" },
    { CryptoAddress_test_main, "CryptoAddress_test" },
    { printIp_test_main, "printIp_test" },
    { IpTunnel_test_main, "IpTunnel_test" },
    { RouteGen_test_main, "RouteGen_test" },
    { Sockaddr_test_main, "Sockaddr_test" },
    { AddrTools_test_main, "AddrTools_test" },
    { AverageRoller_test_main, "AverageRoller_test" },
    { Base10_test_main, "Base10_test" },
    { Base32_test_main, "Base32_test" },
    { Bits_test_main, "Bits_test" },
    { Checksum_test_main, "Checksum_test" },
    { Endian_test_main, "Endian_test" },
    { Hex_test_main, "Hex_test" },
    { Identity_test_main, "Identity_test" },
    { Map_test_main, "Map_test" },
    { Process_test_main, "Process_test" },
    { QSort_test_main, "QSort_test" },
    { Seccomp_test_main, "Seccomp_test" },
    { Set_test_main, "Set_test" },
    { VarInt_test_main, "VarInt_test" }
};
static const int TEST_COUNT = (int) (sizeof(TESTS) / sizeof(*TESTS));

static const struct {
    FuzzTestInit init;
    FuzzTest fuzz;
    char* name;
} FUZZ_TESTS[] = {
    { JsonBencMessageReader_init, JsonBencMessageReader_fuzz, "JsonBencMessageReader_fuzz_test" },
    { CryptoAuth_init, CryptoAuth_fuzz, "CryptoAuth_fuzz_test" },
    { FramingIface_init, FramingIface_fuzz, "FramingIface_fuzz_test" },
    { Main_init, Main_fuzz, "Main_fuzz_test" },
    { Map_init, Map_fuzz, "Map_fuzz_test" }
};
static const int FUZZ_TEST_COUNT = (int) (sizeof(FUZZ_TESTS) / sizeof(*FUZZ_TESTS));

static const char* FUZZ_CASES[] = {
    "benc/serialization/json/test/JsonBencMessageReader_fuzz_test_cases/ConfFile.hex",
    "crypto/test/CryptoAuth_fuzz_test_cases/Default.hex",
    "interface/test/FramingIface_fuzz_test_cases/Default.hex",
    "test/Main_fuzz_test_cases/CtrlAddrErr.hex",
    "test/Main_fuzz_test_cases/CtrlAuthErr.hex",
    "test/Main_fuzz_test_cases/CtrlPing.hex",
    "test/Main_fuzz_test_cases/CtrlPong.hex",
    "test/Main_fuzz_test_cases/CtrlUndeliverable.hex",
    "test/Main_fuzz_test_cases/DhtFindNodeQuery.hex",
    "test/Main_fuzz_test_cases/DhtGetPeersQuery.hex",
    "test/Main_fuzz_test_cases/DhtPingQuery.hex",
    "util/test/Map_fuzz_test_cases/Default.hex"
};
static const int FUZZ_CASE_COUNT = (int) (sizeof(FUZZ_CASES) / sizeof(*FUZZ_CASES));

// Index into FUZZ_TESTS[] for each entry of FUZZ_CASES[].
// Each recorded fuzz case is the raw input for its own fuzz test, it does not
// carry a selector in front of the message.
static const int FUZZ_CASE_TEST[] = {
    0, // ConfFile.hex   (JsonBencMessageReader)
    1, // Default.hex    (CryptoAuth)
    2, // Default.hex    (FramingIface)
    3, 3, 3, 3, 3, 3, 3, 3, 3, // (Main)
    4  // Default.hex    (Map)
};

static uint64_t runTest(Test test,
                        char* name,
                        uint64_t startTime,
                        int argc,
                        char** argv,
                        int quiet)
{
    if (!quiet) { fprintf(stderr, "Running test %s", name); }
    Assert_true(!test(argc, argv));
    if (!quiet) {
        uint64_t now = Time_hrtime();
        char* seventySpaces =
            "                                                                      ";
        int count = CString_strlen(name);
        if (count > 69) { count = 69; }
        fprintf(stderr, "%s%d.%d ms\n",
                &seventySpaces[count],
                (int)((now - startTime)/1000000),
                (int)((now - startTime)/1000)%1000);
        return now;
    }
    return startTime;
}

static void usage(char* appName)
{
    printf("%s <test>     run one test\n", appName);
    printf("%s all        run every test\n\n", appName);
    printf("Flags:\n");
    printf("  --quiet                # Don't write test timings to stderr");
    printf("  --stderr-to <path>     # Redirect stderr to a file (append mode)");
    printf("  --inittests            # When using `fuzz` first initialize ALL tests");
    printf("\n");
    printf("Available Tests:\n");
    for (int i = 0; i < TEST_COUNT; i++) {
        printf("%s %s\n", appName, TESTS[i].name);
    }
    printf("\nAvailable Fuzz Tests:\n");
    for (int i = 0; i < FUZZ_CASE_COUNT; i++) {
        printf("%s fuzz < %s\n", appName, FUZZ_CASES[i]);
    }
}

// The saved fuzz cases are text files full of hex with '#' comment lines.
// Lines starting with '#' are skipped and the remaining hex is decoded.
static void readFile(int fileNo, struct Allocator* alloc, struct Message* fuzz)
{
    char* hex = Allocator_malloc(alloc, 1<<16);
    ssize_t length = read(fileNo, hex, (1<<16) - 1);
    if (length <= 0) { fuzz->length = 0; return; }
    hex[length] = '\0';
    char* out = Allocator_malloc(alloc, 1<<15);
    int o = 0;
    for (int i = 0; i < length; i++) {
        if (i == 0 || hex[i-1] == '\n') {
            while (hex[i] == ' ' || hex[i] == '\t') { i++; }
            if (hex[i] == '#') { while (hex[i] != '\n' && i < length) { i++; } continue; }
        }
        if (hex[i] != '\n' && hex[i] != '\r' && hex[i] != ' ' && hex[i] != '\t') {
            out[o++] = hex[i];
        }
    }
    out[o] = '\0';
    uint8_t* bin = Allocator_malloc(alloc, 1<<15);
    int binLen = Hex_decode(bin, 1<<15, (uint8_t*)out, o);
    Assert_true(binLen >= 0);
    if (binLen > (int)fuzz->capacity - (int)fuzz->padding) {
        printf("No test files over [%d] bytes\n", fuzz->capacity - fuzz->padding);
        binLen = 0;
    }
    Bits_memcpy(fuzz->bytes, bin, binLen);
    fuzz->length = binLen;
}

// Run the recorded fuzz input against its own fuzz test. The input does not
// carry a selector prefix, the mapping is fixed by FUZZ_CASE_TEST[].
static void runFuzzCase(const char* testCase,
                        struct Allocator* alloc,
                        struct Random* rand,
                        int quiet)
{
    for (int i = 0; i < FUZZ_CASE_COUNT; i++) {
        if (!CString_strcmp(FUZZ_CASES[i], testCase)) {
            if (!quiet) { fprintf(stderr, "Running fuzz %s", testCase); }
            void* ctx = FUZZ_TESTS[FUZZ_CASE_TEST[i]].init(alloc, rand);
            struct Message* fuzz = Message_new(4096, 128, alloc);
            int f = open(testCase, O_RDONLY);
            Assert_true(f > -1);
            readFile(f, alloc, fuzz);
            close(f);
            FUZZ_TESTS[FUZZ_CASE_TEST[i]].fuzz(ctx, fuzz);
            return;
        }
    }
    Assert_failure("unknown fuzz case");
}

static void** initFuzzTests(struct Allocator* alloc, struct Random* rand)
{
    void** contexts = Allocator_calloc(alloc, sizeof(char*), FUZZ_TEST_COUNT);
    for (int i = 0; i < FUZZ_TEST_COUNT; i++) {
        contexts[i] = FUZZ_TESTS[i].init(alloc, rand);
    }
    return contexts;
}

static int runFuzzTest(
    void** ctxs,
    struct Allocator* alloc,
    struct Random* rand,
    struct Message* fuzz,
    const char* testCase,
    int quiet)
{
    if (fuzz->length < 4) { return 100; }
    uint32_t selector = Er_assert(Message_epop32be(fuzz));
    if (selector >= (uint32_t)FUZZ_TEST_COUNT) {
        printf("selector [%x] out of bounds [%u]\n", selector, FUZZ_TEST_COUNT);
        return 100;
    }
    if (!testCase) { testCase = FUZZ_TESTS[selector].name; }
    if (!quiet) { fprintf(stderr, "Running fuzz %s", testCase); }
    void* ctx = ctxs ? ctxs[selector] : FUZZ_TESTS[selector].init(alloc, rand);
    FUZZ_TESTS[selector].fuzz(ctx, fuzz);
    return 0;
}

static uint64_t runFuzzTestManual(
    struct Allocator* alloc,
    struct Random* detRand,
    const char* testCase,
    uint64_t startTime,
    int quiet)
{
    runFuzzCase(testCase, alloc, detRand, quiet);

    if (!quiet) {
        uint64_t now = Time_hrtime();
        char* seventySpaces =
            "                                                                      ";
        int count = CString_strlen(testCase);
        if (count > 69) { count = 69; }
        fprintf(stderr, "%s%d.%d ms\n",
                &seventySpaces[count],
                (int)((now - startTime)/1000000),
                (int)((now - startTime)/1000)%1000);
        return now;
    }
    return startTime;
}

// We don't really want to let AFL write the random seed because the amount of mixing
// that occurs between the input and output makes any attempt at optimizing the seed
// useless.
//
// We do, however, want to make sure that crashes discovered by AFL are reproducable.
//
// We might imagine letting part of the AFL message content be the random data which
// is returned, but we don't know how much will be requested in advance. Possibly
// something for the future.
#define RANDOM_SEED "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"

static int fuzzMain(struct Allocator* alloc, struct Random* detRand, int initTests, int quiet)
{
    struct Message* fuzz = Message_new(4096, 128, alloc);

#ifdef __AFL_INIT
    // Enable AFL deferred forkserver mode. Requires compilation using afl-clang-fast
    initTests = 1;
#endif

    void** ctxs = (initTests) ? initFuzzTests(alloc, detRand) : NULL;

#ifdef __AFL_INIT
    __AFL_INIT();
#endif

    readFile(STDIN_FILENO, alloc, fuzz);
    int out = runFuzzTest(ctxs, alloc, detRand, fuzz, NULL, quiet);
    printf("\n");
    return out;
}

static void stderrTo(char* file)
{
    int f = open(file, O_CREAT | O_APPEND | O_WRONLY, 0666);
    Assert_true(f > -1);
    Assert_true(dup2(f, STDERR_FILENO) == STDERR_FILENO);
}

static int main2(int argc, char** argv, struct Allocator* alloc, struct Random* detRand)
{
    int initTests = 0;
    int quiet = 0;
    for (int i = 0; i < argc; i++) {
        if (!CString_strcmp("--inittests", argv[i])) { initTests = 1; }
        if (!CString_strcmp("--stderr-to", argv[i]) && argc > i + 1) { stderrTo(argv[i + 1]); }
        if (!CString_strcmp("--quiet", argv[i])) { quiet = 1; }
    }
    if (argc > 1 && !CString_strcmp("fuzz", argv[1])) {
        return fuzzMain(alloc, detRand, initTests, quiet);
    }
    uint64_t now = Time_hrtime();
    uint64_t startTime = now;
    if (argc < 2) {
        Assert_true(argc > 0);
        usage(argv[0]);
        return 100;
    }
    if (argc > 1 && CString_strcmp("all", argv[1])) {
        for (int i = 0; i < TEST_COUNT; i++) {
            if (!CString_strcmp(TESTS[i].name, argv[1])) {
                TESTS[i].func(argc, argv);
                return 0;
            }
        }
        for (int i = 0; i < FUZZ_CASE_COUNT; i++) {
            if (!CString_strcmp(FUZZ_CASES[i], argv[1])) {
                runFuzzTestManual(alloc, detRand, FUZZ_CASES[i], now, quiet);
                return 0;
            }
        }
        usage(argv[0]);
        return 100;
    }
    for (int i = 0; i < TEST_COUNT; i++) {
        now = runTest(TESTS[i].func, TESTS[i].name, now, argc, argv, quiet);
    }
    for (int i = 0; i < FUZZ_CASE_COUNT; i++) {
        // TODO(hyperboria): Apparently a race condition in the allocator
        // if you have async freeing in progress and then you come in and
        // free the root allocator, you get an assertion.
        //
        //struct Allocator* child = Allocator_child(alloc);
        struct Allocator* child = MallocAllocator_new(1<<24);

        now = runFuzzTestManual(child, detRand, FUZZ_CASES[i], now, quiet);
        Allocator_free(child);
    }
    if (!quiet) {
        fprintf(stderr, "Total test time %d.%d ms\n",
                (int)((now - startTime)/1000000),
                (int)((now - startTime)/1000)%1000);
    }
    return 0;
}

int main(int argc, char** argv)
{
    struct Allocator* alloc = MallocAllocator_new(1<<24);
    struct RandomSeed* rs = DeterminentRandomSeed_new(alloc, RANDOM_SEED);
    struct Random* detRand = Random_newWithSeed(alloc, NULL, rs, NULL);
    int out = main2(argc, argv, alloc, detRand);
    Allocator_free(alloc);
    return out;
}
