AUTOMAKE_OPTIONS = foreign subdir-objects
ACLOCAL_AMFLAGS = -I m4

# ---- compilation model --------------------------------------------------
#
# cjdns passes its own set of flags; the platform/logging/safety defines all
# get folded into AM_CPPFLAGS via configure substitution.
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/nacl/include \
	-I$(top_srcdir)/libuv/include $(CJDNS_CPPFLAGS)

AM_CFLAGS = $(CJDNS_OPTFLAGS) -std=gnu99 -Wall -Wextra -Werror \
	-Wno-pointer-sign -Wmissing-prototypes -pedantic -Wno-unused-parameter \
	-Wno-gnu-zero-variadic-macro-arguments -Wno-variadic-macro-arguments-omitted \
	$(CJDNS_WARN_FLAGS) -fomit-frame-pointer $(CJDNS_CFLAGS_SPLIT)

AM_LDFLAGS = $(CJDNS_PIE_LDFLAGS)

# ---- vendored libuv ------------------------------------------------------
.PHONY: libuv
libuv:
	@if test ! -x $(top_srcdir)/libuv/configure; then \
		cd $(top_srcdir)/libuv && ./autogen.sh; \
	fi
	cd $(top_srcdir)/libuv && ./configure --disable-shared --enable-static \
		CFLAGS="$(CFLAGS) -fPIC"
	$(MAKE) -C $(top_srcdir)/libuv

# ---- nacl: pruned djb NaCl ref implementations --------------------------
#
# The ref sources are written against "abstract" symbol names (crypto_box(),
# crypto_stream(), ...).  Each convenience library compiles them with -D
# renames so the abstract names resolve to the concrete primitive symbols
# that the rest of cjdns actually calls.  No -Werror here: this is vendored
# public-domain code, tolerate whatever the current compiler says.
NACL_CFLAGS = $(CJDNS_OPTFLAGS) -std=gnu99 -Wall -Wextra -Wno-pointer-sign \
	-Wno-unused-parameter -Wno-unterminated-string-initialization \
	-Wno-sign-compare -Wno-overflow -pedantic
NACL_CPPFLAGS = -I$(top_srcdir)/nacl/include

noinst_LTLIBRARIES = \
	libnacl/libbox.la \
	libnacl/libsecretbox.la \
	libnacl/libscalarmult.la \
	libnacl/libcore_hsalsa20.la \
	libnacl/libcore_salsa20.la \
	libnacl/libstream_salsa20.la \
	libnacl/libstream_xsalsa20.la \
	libnacl/libonetimeauth_poly1305.la \
	libnacl/libverify_16.la \
	libnacl/libverify_32.la \
	libnacl/libhash_sha512.la \
	libnacl/libhash_sha256.la \
	libnacl/libhashblocks_sha512.la \
	libnacl/libhashblocks_sha256.la \
	libnacl/librandombytes.la

BOX_RENAME = -Dcrypto_box=crypto_box_curve25519xsalsa20poly1305 \
	-Dcrypto_box_open=crypto_box_curve25519xsalsa20poly1305_open \
	-Dcrypto_box_beforenm=crypto_box_curve25519xsalsa20poly1305_beforenm \
	-Dcrypto_box_afternm=crypto_box_curve25519xsalsa20poly1305_afternm \
	-Dcrypto_box_open_afternm=crypto_box_curve25519xsalsa20poly1305_open_afternm \
	-Dcrypto_box_keypair=crypto_box_curve25519xsalsa20poly1305_keypair \
	-Dcrypto_box_PUBLICKEYBYTES=crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES \
	-Dcrypto_box_SECRETKEYBYTES=crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES \
	-Dcrypto_box_BEFORENMBYTES=32 \
	-Dcrypto_box_NONCEBYTES=crypto_box_curve25519xsalsa20poly1305_NONCEBYTES \
	-Dcrypto_box_ZEROBYTES=crypto_box_curve25519xsalsa20poly1305_ZEROBYTES \
	-Dcrypto_box_BOXZEROBYTES=crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES

libnacl_libbox_la_SOURCES = \
	nacl/crypto_box/box.c \
	nacl/crypto_box/before.c \
	nacl/crypto_box/after.c \
	nacl/crypto_box/keypair.c
libnacl_libbox_la_CPPFLAGS = $(NACL_CPPFLAGS) $(BOX_RENAME)
libnacl_libbox_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libsecretbox_la_SOURCES = nacl/crypto_secretbox/box.c
libnacl_libsecretbox_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_secretbox=crypto_secretbox_xsalsa20poly1305 \
	-Dcrypto_secretbox_open=crypto_secretbox_xsalsa20poly1305_open \
	-Dcrypto_secretbox_KEYBYTES=crypto_secretbox_xsalsa20poly1305_KEYBYTES \
	-Dcrypto_secretbox_NONCEBYTES=crypto_secretbox_xsalsa20poly1305_NONCEBYTES \
	-Dcrypto_secretbox_ZEROBYTES=crypto_secretbox_xsalsa20poly1305_ZEROBYTES \
	-Dcrypto_secretbox_BOXZEROBYTES=crypto_secretbox_xsalsa20poly1305_BOXZEROBYTES
libnacl_libsecretbox_la_CFLAGS = $(NACL_CFLAGS)

SCALARMULT_SRC = \
	nacl/crypto_scalarmult/scalarmult.c \
	nacl/crypto_scalarmult/base.c \
	nacl/crypto_scalarmult/fe_0.c \
	nacl/crypto_scalarmult/fe_1.c \
	nacl/crypto_scalarmult/fe_add.c \
	nacl/crypto_scalarmult/fe_copy.c \
	nacl/crypto_scalarmult/fe_cswap.c \
	nacl/crypto_scalarmult/fe_frombytes.c \
	nacl/crypto_scalarmult/fe_invert.c \
	nacl/crypto_scalarmult/fe_mul.c \
	nacl/crypto_scalarmult/fe_mul121666.c \
	nacl/crypto_scalarmult/fe_sq.c \
	nacl/crypto_scalarmult/fe_sub.c \
	nacl/crypto_scalarmult/fe_tobytes.c
libnacl_libscalarmult_la_SOURCES = $(SCALARMULT_SRC)
libnacl_libscalarmult_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_scalarmult=crypto_scalarmult_curve25519 \
	-Dcrypto_scalarmult_base=crypto_scalarmult_curve25519_base \
	-Dcrypto_scalarmult_BYTES=crypto_scalarmult_curve25519_BYTES \
	-Dcrypto_scalarmult_SCALARBYTES=crypto_scalarmult_curve25519_SCALARBYTES
libnacl_libscalarmult_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libcore_hsalsa20_la_SOURCES = nacl/crypto_core_hsalsa20/core.c
libnacl_libcore_hsalsa20_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_core=crypto_core_hsalsa20 \
	-Dcrypto_core_OUTPUTBYTES=crypto_core_hsalsa20_OUTPUTBYTES \
	-Dcrypto_core_INPUTBYTES=crypto_core_hsalsa20_INPUTBYTES \
	-Dcrypto_core_KEYBYTES=crypto_core_hsalsa20_KEYBYTES \
	-Dcrypto_core_CONSTBYTES=crypto_core_hsalsa20_CONSTBYTES
libnacl_libcore_hsalsa20_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libcore_salsa20_la_SOURCES = nacl/crypto_core_salsa20/core.c
libnacl_libcore_salsa20_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_core=crypto_core_salsa20 \
	-Dcrypto_core_OUTPUTBYTES=crypto_core_salsa20_OUTPUTBYTES \
	-Dcrypto_core_INPUTBYTES=crypto_core_salsa20_INPUTBYTES \
	-Dcrypto_core_KEYBYTES=crypto_core_salsa20_KEYBYTES \
	-Dcrypto_core_CONSTBYTES=crypto_core_salsa20_CONSTBYTES
libnacl_libcore_salsa20_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libstream_salsa20_la_SOURCES = nacl/crypto_stream_salsa20/stream.c nacl/crypto_stream_salsa20/xor.c
libnacl_libstream_salsa20_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_stream=crypto_stream_salsa20 \
	-Dcrypto_stream_xor=crypto_stream_salsa20_xor \
	-Dcrypto_stream_KEYBYTES=crypto_stream_salsa20_KEYBYTES \
	-Dcrypto_stream_NONCEBYTES=crypto_stream_salsa20_NONCEBYTES
libnacl_libstream_salsa20_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libstream_xsalsa20_la_SOURCES = nacl/crypto_stream_xsalsa20/stream.c nacl/crypto_stream_xsalsa20/xor.c
libnacl_libstream_xsalsa20_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_stream=crypto_stream_xsalsa20 \
	-Dcrypto_stream_xor=crypto_stream_xsalsa20_xor \
	-Dcrypto_stream_KEYBYTES=crypto_stream_xsalsa20_KEYBYTES \
	-Dcrypto_stream_NONCEBYTES=crypto_stream_xsalsa20_NONCEBYTES
libnacl_libstream_xsalsa20_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libonetimeauth_poly1305_la_SOURCES = nacl/crypto_onetimeauth_poly1305/auth.c nacl/crypto_onetimeauth_poly1305/verify.c
libnacl_libonetimeauth_poly1305_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_onetimeauth=crypto_onetimeauth_poly1305 \
	-Dcrypto_onetimeauth_verify=crypto_onetimeauth_poly1305_verify \
	-Dcrypto_onetimeauth_BYTES=crypto_onetimeauth_poly1305_BYTES \
	-Dcrypto_onetimeauth_KEYBYTES=crypto_onetimeauth_poly1305_KEYBYTES
libnacl_libonetimeauth_poly1305_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libverify_16_la_SOURCES = nacl/crypto_verify_16/verify.c
libnacl_libverify_16_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_verify=crypto_verify_16 \
	-Dcrypto_verify_BYTES=crypto_verify_16_BYTES
libnacl_libverify_16_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libverify_32_la_SOURCES = nacl/crypto_verify_32/verify.c
libnacl_libverify_32_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_verify=crypto_verify_32 \
	-Dcrypto_verify_BYTES=crypto_verify_32_BYTES
libnacl_libverify_32_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libhash_sha512_la_SOURCES = nacl/crypto_hash_sha512/hash.c
libnacl_libhash_sha512_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_hash=crypto_hash_sha512 \
	-Dcrypto_hash_BYTES=crypto_hash_sha512_BYTES
libnacl_libhash_sha512_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libhash_sha256_la_SOURCES = nacl/crypto_hash_sha256/hash.c
libnacl_libhash_sha256_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_hash=crypto_hash_sha256 \
	-Dcrypto_hash_BYTES=crypto_hash_sha256_BYTES
libnacl_libhash_sha256_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libhashblocks_sha512_la_SOURCES = nacl/crypto_hashblocks_sha512/blocks.c
libnacl_libhashblocks_sha512_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_hashblocks=crypto_hashblocks_sha512
libnacl_libhashblocks_sha512_la_CFLAGS = $(NACL_CFLAGS)

libnacl_libhashblocks_sha256_la_SOURCES = nacl/crypto_hashblocks_sha256/blocks.c
libnacl_libhashblocks_sha256_la_CPPFLAGS = $(NACL_CPPFLAGS) \
	-Dcrypto_hashblocks=crypto_hashblocks_sha256
libnacl_libhashblocks_sha256_la_CFLAGS = $(NACL_CFLAGS)

libnacl_librandombytes_la_SOURCES = nacl/randombytes/devurandom.c
libnacl_librandombytes_la_CPPFLAGS = $(NACL_CPPFLAGS)
libnacl_librandombytes_la_CFLAGS = $(NACL_CFLAGS)
# ---- programs ----

NACL_LIBS = \
	$(top_builddir)/libnacl/libbox.la \
	$(top_builddir)/libnacl/libsecretbox.la \
	$(top_builddir)/libnacl/libscalarmult.la \
	$(top_builddir)/libnacl/libonetimeauth_poly1305.la \
	$(top_builddir)/libnacl/libstream_xsalsa20.la \
	$(top_builddir)/libnacl/libstream_salsa20.la \
	$(top_builddir)/libnacl/libhash_sha512.la \
	$(top_builddir)/libnacl/libhash_sha256.la \
	$(top_builddir)/libnacl/libverify_16.la \
	$(top_builddir)/libnacl/libverify_32.la \
	$(top_builddir)/libnacl/libcore_hsalsa20.la \
	$(top_builddir)/libnacl/libcore_salsa20.la \
	$(top_builddir)/libnacl/libhashblocks_sha512.la \
	$(top_builddir)/libnacl/libhashblocks_sha256.la \
	$(top_builddir)/libnacl/librandombytes.la

bin_PROGRAMS = cjdroute publictoip6 privatetopublic sybilsim makekeys mkpasswd randombytes testcjdroute

cjdroute_SOURCES = \
        admin/Admin.c \
        admin/AdminLog.c \
        admin/AuthorizedPasswords.c \
        admin/angel/Core.c \
        admin/angel/InterfaceWaiter.c \
        benc/Dict.c \
        benc/List.c \
        benc/String.c \
        benc/serialization/cloner/Cloner.c \
        benc/serialization/json/JsonBencMessageReader.c \
        benc/serialization/json/JsonBencSerializer.c \
        benc/serialization/standard/BencMessageReader.c \
        benc/serialization/standard/BencMessageWriter.c \
        client/AdminClient.c \
        client/Configurator.c \
        client/cjdroute2.c \
        crypto/AddressCalc.c \
        crypto/CryptoAuth.c \
        crypto/Key.c \
        crypto/Sign.c \
        crypto/Sign_admin.c \
        crypto/random/Random.c \
        crypto/random/nanotime/NanotimeEntropyProvider.c \
        crypto/random/seed/DevUrandomRandomSeed.c \
        crypto/random/seed/GetEntropyRandomSeed.c \
        crypto/random/seed/LinuxRandomUuidSysctlRandomSeed.c \
        crypto/random/seed/ProcSysKernelRandomUuidRandomSeed.c \
        crypto/random/seed/RandomSeed.c \
        crypto/random/seed/SystemRandomSeed.c \
        crypto/sign/fe_0.c \
        crypto/sign/fe_1.c \
        crypto/sign/fe_add.c \
        crypto/sign/fe_cmov.c \
        crypto/sign/fe_copy.c \
        crypto/sign/fe_frombytes.c \
        crypto/sign/fe_invert.c \
        crypto/sign/fe_isnegative.c \
        crypto/sign/fe_isnonzero.c \
        crypto/sign/fe_mul.c \
        crypto/sign/fe_neg.c \
        crypto/sign/fe_pow22523.c \
        crypto/sign/fe_sq.c \
        crypto/sign/fe_sq2.c \
        crypto/sign/fe_sub.c \
        crypto/sign/fe_tobytes.c \
        crypto/sign/ge_add.c \
        crypto/sign/ge_double_scalarmult.c \
        crypto/sign/ge_frombytes.c \
        crypto/sign/ge_madd.c \
        crypto/sign/ge_msub.c \
        crypto/sign/ge_p1p1_to_p2.c \
        crypto/sign/ge_p1p1_to_p3.c \
        crypto/sign/ge_p2_0.c \
        crypto/sign/ge_p2_dbl.c \
        crypto/sign/ge_p3_0.c \
        crypto/sign/ge_p3_dbl.c \
        crypto/sign/ge_p3_to_cached.c \
        crypto/sign/ge_p3_to_p2.c \
        crypto/sign/ge_p3_tobytes.c \
        crypto/sign/ge_precomp_0.c \
        crypto/sign/ge_scalarmult_base.c \
        crypto/sign/ge_sub.c \
        crypto/sign/ge_tobytes.c \
        crypto/sign/open.c \
        crypto/sign/sc_muladd.c \
        crypto/sign/sc_reduce.c \
        dht/Address.c \
        dht/DHTModuleRegistry.c \
        dht/EncodingSchemeModule.c \
        dht/Pathfinder.c \
        dht/ReplyModule.c \
        dht/SerializationModule.c \
        dht/dhtcore/Janitor.c \
        dht/dhtcore/Janitor_admin.c \
        dht/dhtcore/Node.c \
        dht/dhtcore/NodeStore.c \
        dht/dhtcore/NodeStore_admin.c \
        dht/dhtcore/ReplySerializer.c \
        dht/dhtcore/Router.c \
        dht/dhtcore/RouterModule.c \
        dht/dhtcore/RouterModule_admin.c \
        dht/dhtcore/RumorMill.c \
        dht/dhtcore/SearchRunner.c \
        dht/dhtcore/SearchRunner_admin.c \
        dht/dhtcore/SearchStore.c \
        dht/dhtcore/VersionList.c \
        exception/Er.c \
        exception/Except.c \
        interface/ASynchronizer.c \
        interface/ETHInterface_admin.c \
        interface/ETHInterface_linux.c \
        interface/UDPInterface.c \
        interface/UDPInterface_admin.c \
        interface/addressable/AddrIfaceMuxer.c \
        interface/addressable/PacketHeaderToUDPAddrIface.c \
        interface/tuntap/AndroidWrapper.c \
        interface/tuntap/SocketInterface.c \
        interface/tuntap/SocketWrapper.c \
        interface/tuntap/TUNInterface_linux.c \
        io/ArrayReader.c \
        io/ArrayWriter.c \
        io/FileWriter.c \
        memory/Allocator.c \
        memory/Allocator_admin.c \
        memory/BufferAllocator.c \
        memory/MallocAllocator.c \
        net/Benchmark.c \
        net/ControlHandler.c \
        net/EventEmitter.c \
        net/InterfaceController.c \
        net/InterfaceController_admin.c \
        net/NetCore.c \
        net/PeerLink.c \
        net/SessionManager.c \
        net/SessionManager_admin.c \
        net/SwitchPinger.c \
        net/SwitchPinger_admin.c \
        net/TUNAdapter.c \
        net/UpperDistributor.c \
        net/UpperDistributor_admin.c \
        switch/EncodingScheme.c \
        switch/SwitchCore.c \
        tunnel/IpTunnel.c \
        tunnel/IpTunnel_admin.c \
        tunnel/RouteGen.c \
        tunnel/RouteGen_admin.c \
        util/AddrTools.c \
        util/ArchInfo.c \
        util/ArrayList.c \
        util/Assert.c \
        util/AverageRoller.c \
        util/Base10.c \
        util/Bits.c \
        util/CString.c \
        util/GlobalConfig.c \
        util/Hex.c \
        util/Pinger.c \
        util/Seccomp_linux.c \
        util/Security.c \
        util/Security_admin.c \
        util/Setuid_linux.c \
        util/SysInfo.c \
        util/events/libuv/Event.c \
        util/events/libuv/EventBase.c \
        util/events/libuv/FakeNetwork.c \
        util/events/libuv/Pipe.c \
        util/events/libuv/PipeServer.c \
        util/events/libuv/Process.c \
        util/events/libuv/Time.c \
        util/events/libuv/Timeout.c \
        util/events/libuv/UDPAddrIface.c \
        util/log/FileWriterLog.c \
        util/log/IndirectLog.c \
        util/log/Log.c \
        util/log/WriterLog.c \
        util/platform/Sockaddr.c \
        util/platform/Socket.c \
        util/platform/netdev/NetDev.c \
        util/platform/netdev/NetPlatform_linux.c \
        util/version/Version.c \
        wire/Message.c \
        crypto/random/test/DeterminentRandomSeed.c


publictoip6_SOURCES = \
        benc/Dict.c \
        benc/List.c \
        benc/String.c \
        contrib/c/publictoip6.c \
        crypto/AddressCalc.c \
        crypto/Key.c \
        crypto/random/Random.c \
        crypto/random/seed/DevUrandomRandomSeed.c \
        crypto/random/seed/GetEntropyRandomSeed.c \
        crypto/random/seed/LinuxRandomUuidSysctlRandomSeed.c \
        crypto/random/seed/ProcSysKernelRandomUuidRandomSeed.c \
        crypto/random/seed/RandomSeed.c \
        crypto/random/seed/SystemRandomSeed.c \
        exception/Er.c \
        exception/Except.c \
        memory/Allocator.c \
        util/AddrTools.c \
        util/Assert.c \
        util/Base10.c \
        util/Bits.c \
        util/CString.c \
        util/Hex.c \
        util/log/Log.c \
        util/platform/Sockaddr.c \
        wire/Message.c

privatetopublic_SOURCES = \
        benc/Dict.c \
        benc/List.c \
        benc/String.c \
        contrib/c/privatetopublic.c \
        crypto/AddressCalc.c \
        crypto/Key.c \
        crypto/random/Random.c \
        crypto/random/seed/DevUrandomRandomSeed.c \
        crypto/random/seed/GetEntropyRandomSeed.c \
        crypto/random/seed/LinuxRandomUuidSysctlRandomSeed.c \
        crypto/random/seed/ProcSysKernelRandomUuidRandomSeed.c \
        crypto/random/seed/RandomSeed.c \
        crypto/random/seed/SystemRandomSeed.c \
        dht/Address.c \
        exception/Er.c \
        exception/Except.c \
        memory/Allocator.c \
        util/AddrTools.c \
        util/Assert.c \
        util/Base10.c \
        util/Bits.c \
        util/CString.c \
        util/Hex.c \
        util/log/Log.c \
        util/platform/Sockaddr.c \
        wire/Message.c

sybilsim_SOURCES = \
        admin/Admin.c \
        admin/AdminLog.c \
        admin/AuthorizedPasswords.c \
        admin/angel/Core.c \
        admin/angel/InterfaceWaiter.c \
        benc/Dict.c \
        benc/List.c \
        benc/String.c \
        benc/serialization/cloner/Cloner.c \
        benc/serialization/json/JsonBencSerializer.c \
        benc/serialization/standard/BencMessageReader.c \
        benc/serialization/standard/BencMessageWriter.c \
        client/AdminClient.c \
        contrib/c/sybilsim.c \
        crypto/AddressCalc.c \
        crypto/CryptoAuth.c \
        crypto/Key.c \
        crypto/Sign.c \
        crypto/Sign_admin.c \
        crypto/random/Random.c \
        crypto/random/nanotime/NanotimeEntropyProvider.c \
        crypto/random/seed/DevUrandomRandomSeed.c \
        crypto/random/seed/GetEntropyRandomSeed.c \
        crypto/random/seed/LinuxRandomUuidSysctlRandomSeed.c \
        crypto/random/seed/ProcSysKernelRandomUuidRandomSeed.c \
        crypto/random/seed/RandomSeed.c \
        crypto/random/seed/SystemRandomSeed.c \
        crypto/sign/fe_0.c \
        crypto/sign/fe_1.c \
        crypto/sign/fe_add.c \
        crypto/sign/fe_cmov.c \
        crypto/sign/fe_copy.c \
        crypto/sign/fe_frombytes.c \
        crypto/sign/fe_invert.c \
        crypto/sign/fe_isnegative.c \
        crypto/sign/fe_isnonzero.c \
        crypto/sign/fe_mul.c \
        crypto/sign/fe_neg.c \
        crypto/sign/fe_pow22523.c \
        crypto/sign/fe_sq.c \
        crypto/sign/fe_sq2.c \
        crypto/sign/fe_sub.c \
        crypto/sign/fe_tobytes.c \
        crypto/sign/ge_add.c \
        crypto/sign/ge_double_scalarmult.c \
        crypto/sign/ge_frombytes.c \
        crypto/sign/ge_madd.c \
        crypto/sign/ge_msub.c \
        crypto/sign/ge_p1p1_to_p2.c \
        crypto/sign/ge_p1p1_to_p3.c \
        crypto/sign/ge_p2_0.c \
        crypto/sign/ge_p2_dbl.c \
        crypto/sign/ge_p3_0.c \
        crypto/sign/ge_p3_dbl.c \
        crypto/sign/ge_p3_to_cached.c \
        crypto/sign/ge_p3_to_p2.c \
        crypto/sign/ge_p3_tobytes.c \
        crypto/sign/ge_precomp_0.c \
        crypto/sign/ge_scalarmult_base.c \
        crypto/sign/ge_sub.c \
        crypto/sign/ge_tobytes.c \
        crypto/sign/open.c \
        crypto/sign/sc_muladd.c \
        crypto/sign/sc_reduce.c \
        dht/Address.c \
        dht/DHTModuleRegistry.c \
        dht/EncodingSchemeModule.c \
        dht/Pathfinder.c \
        dht/ReplyModule.c \
        dht/SerializationModule.c \
        dht/dhtcore/Janitor.c \
        dht/dhtcore/Janitor_admin.c \
        dht/dhtcore/Node.c \
        dht/dhtcore/NodeStore.c \
        dht/dhtcore/NodeStore_admin.c \
        dht/dhtcore/ReplySerializer.c \
        dht/dhtcore/Router.c \
        dht/dhtcore/RouterModule.c \
        dht/dhtcore/RouterModule_admin.c \
        dht/dhtcore/RumorMill.c \
        dht/dhtcore/SearchRunner.c \
        dht/dhtcore/SearchRunner_admin.c \
        dht/dhtcore/SearchStore.c \
        dht/dhtcore/VersionList.c \
        exception/Er.c \
        exception/Except.c \
        interface/ASynchronizer.c \
        interface/ETHInterface_admin.c \
        interface/UDPInterface.c \
        interface/UDPInterface_admin.c \
        interface/addressable/AddrIfaceAdapter.c \
        interface/addressable/AddrIfaceMuxer.c \
        interface/addressable/PacketHeaderToUDPAddrIface.c \
        interface/tuntap/AndroidWrapper.c \
        interface/tuntap/SocketInterface.c \
        interface/tuntap/SocketWrapper.c \
        io/ArrayReader.c \
        io/ArrayWriter.c \
        io/FileReader.c \
        io/FileWriter.c \
        memory/Allocator.c \
        memory/Allocator_admin.c \
        memory/BufferAllocator.c \
        memory/MallocAllocator.c \
        net/ControlHandler.c \
        net/EventEmitter.c \
        net/InterfaceController.c \
        net/InterfaceController_admin.c \
        net/NetCore.c \
        net/PeerLink.c \
        net/SessionManager.c \
        net/SessionManager_admin.c \
        net/SwitchPinger.c \
        net/SwitchPinger_admin.c \
        net/TUNAdapter.c \
        net/UpperDistributor.c \
        net/UpperDistributor_admin.c \
        switch/EncodingScheme.c \
        switch/SwitchCore.c \
        tunnel/IpTunnel.c \
        tunnel/IpTunnel_admin.c \
        tunnel/RouteGen.c \
        tunnel/RouteGen_admin.c \
        util/AddrTools.c \
        util/ArrayList.c \
        util/Assert.c \
        util/AverageRoller.c \
        util/Base10.c \
        util/Bits.c \
        util/CString.c \
        util/GlobalConfig.c \
        util/Hex.c \
        util/Pinger.c \
        util/Security.c \
        util/Security_admin.c \
        util/events/libuv/EventBase.c \
        util/events/libuv/FakeNetwork.c \
        util/events/libuv/Pipe.c \
        util/events/libuv/PipeServer.c \
        util/events/libuv/Time.c \
        util/events/libuv/Timeout.c \
        util/events/libuv/UDPAddrIface.c \
        util/log/FileWriterLog.c \
        util/log/IndirectLog.c \
        util/log/Log.c \
        util/log/WriterLog.c \
        util/platform/Sockaddr.c \
        util/platform/netdev/NetDev.c \
        util/version/Version.c \
        interface/ETHInterface_linux.c \
        interface/tuntap/TUNInterface_linux.c \
        util/ArchInfo.c \
        util/Seccomp_linux.c \
        util/Setuid_linux.c \
        util/platform/netdev/NetPlatform_linux.c \
        util/events/libuv/Event.c \
        util/platform/Socket.c \
        wire/Message.c

makekeys_SOURCES = \
        benc/Dict.c \
        benc/List.c \
        benc/String.c \
        contrib/c/makekeys.c \
        crypto/AddressCalc.c \
        crypto/Key.c \
        crypto/random/Random.c \
        crypto/random/seed/DevUrandomRandomSeed.c \
        crypto/random/seed/GetEntropyRandomSeed.c \
        crypto/random/seed/LinuxRandomUuidSysctlRandomSeed.c \
        crypto/random/seed/ProcSysKernelRandomUuidRandomSeed.c \
        crypto/random/seed/RandomSeed.c \
        crypto/random/seed/SystemRandomSeed.c \
        exception/Er.c \
        exception/Except.c \
        memory/Allocator.c \
        memory/MallocAllocator.c \
        util/AddrTools.c \
        util/Assert.c \
        util/Base10.c \
        util/Bits.c \
        util/CString.c \
        util/Hex.c \
        util/log/Log.c \
        util/platform/Sockaddr.c \
        wire/Message.c

mkpasswd_SOURCES = \
        contrib/c/mkpasswd.c \
        crypto/random/Random.c \
        crypto/random/seed/DevUrandomRandomSeed.c \
        crypto/random/seed/GetEntropyRandomSeed.c \
        crypto/random/seed/LinuxRandomUuidSysctlRandomSeed.c \
        crypto/random/seed/ProcSysKernelRandomUuidRandomSeed.c \
        crypto/random/seed/RandomSeed.c \
        crypto/random/seed/SystemRandomSeed.c \
        exception/Er.c \
        exception/Except.c \
        memory/Allocator.c \
        memory/MallocAllocator.c \
        util/Assert.c \
        util/Bits.c \
        util/CString.c \
        util/Hex.c \
        util/log/Log.c

randombytes_SOURCES = \
        crypto/random/Random.c \
        crypto/random/randombytes.c \
        crypto/random/seed/DevUrandomRandomSeed.c \
        crypto/random/seed/GetEntropyRandomSeed.c \
        crypto/random/seed/LinuxRandomUuidSysctlRandomSeed.c \
        crypto/random/seed/ProcSysKernelRandomUuidRandomSeed.c \
        crypto/random/seed/RandomSeed.c \
        crypto/random/seed/SystemRandomSeed.c \
        exception/Er.c \
        exception/Except.c \
        memory/Allocator.c \
        memory/MallocAllocator.c \
        util/Assert.c \
        util/Bits.c \
        util/CString.c \
        util/Hex.c \
        util/log/Log.c

testcjdroute_SOURCES = \
        admin/Admin.c \
        benc/Dict.c \
        benc/List.c \
        benc/String.c \
        benc/serialization/json/JsonBencMessageReader.c \
        benc/serialization/standard/BencMessageReader.c \
        benc/serialization/standard/BencMessageWriter.c \
        crypto/AddressCalc.c \
        crypto/CryptoAuth.c \
        crypto/Key.c \
        crypto/Sign.c \
        crypto/random/Random.c \
        crypto/random/seed/DevUrandomRandomSeed.c \
        crypto/random/seed/GetEntropyRandomSeed.c \
        crypto/random/seed/LinuxRandomUuidSysctlRandomSeed.c \
        crypto/random/seed/ProcSysKernelRandomUuidRandomSeed.c \
        crypto/random/seed/RandomSeed.c \
        crypto/random/seed/SystemRandomSeed.c \
        crypto/random/test/DeterminentRandomSeed.c \
        crypto/sign/fe_0.c \
        crypto/sign/fe_1.c \
        crypto/sign/fe_add.c \
        crypto/sign/fe_cmov.c \
        crypto/sign/fe_copy.c \
        crypto/sign/fe_frombytes.c \
        crypto/sign/fe_invert.c \
        crypto/sign/fe_isnegative.c \
        crypto/sign/fe_isnonzero.c \
        crypto/sign/fe_mul.c \
        crypto/sign/fe_neg.c \
        crypto/sign/fe_pow22523.c \
        crypto/sign/fe_sq.c \
        crypto/sign/fe_sq2.c \
        crypto/sign/fe_sub.c \
        crypto/sign/fe_tobytes.c \
        crypto/sign/ge_add.c \
        crypto/sign/ge_double_scalarmult.c \
        crypto/sign/ge_frombytes.c \
        crypto/sign/ge_madd.c \
        crypto/sign/ge_msub.c \
        crypto/sign/ge_p1p1_to_p2.c \
        crypto/sign/ge_p1p1_to_p3.c \
        crypto/sign/ge_p2_0.c \
        crypto/sign/ge_p2_dbl.c \
        crypto/sign/ge_p3_0.c \
        crypto/sign/ge_p3_dbl.c \
        crypto/sign/ge_p3_to_cached.c \
        crypto/sign/ge_p3_to_p2.c \
        crypto/sign/ge_p3_tobytes.c \
        crypto/sign/ge_precomp_0.c \
        crypto/sign/ge_scalarmult_base.c \
        crypto/sign/ge_sub.c \
        crypto/sign/ge_tobytes.c \
        crypto/sign/open.c \
        crypto/sign/sc_muladd.c \
        crypto/sign/sc_reduce.c \
        dht/Address.c \
        dht/DHTModuleRegistry.c \
        dht/EncodingSchemeModule.c \
        dht/Pathfinder.c \
        dht/ReplyModule.c \
        dht/SerializationModule.c \
        dht/dhtcore/Janitor.c \
        dht/dhtcore/Janitor_admin.c \
        dht/dhtcore/Node.c \
        dht/dhtcore/NodeStore.c \
        dht/dhtcore/NodeStore_admin.c \
        dht/dhtcore/ReplySerializer.c \
        dht/dhtcore/Router.c \
        dht/dhtcore/RouterModule.c \
        dht/dhtcore/RouterModule_admin.c \
        dht/dhtcore/RumorMill.c \
        dht/dhtcore/SearchRunner.c \
        dht/dhtcore/SearchRunner_admin.c \
        dht/dhtcore/SearchStore.c \
        dht/dhtcore/VersionList.c \
        exception/Er.c \
        exception/Except.c \
        interface/ASynchronizer.c \
        interface/FramingIface.c \
        interface/tuntap/ARPServer.c \
        interface/tuntap/BSDMessageTypeWrapper.c \
        interface/tuntap/NDPServer.c \
        interface/tuntap/TAPWrapper.c \
        interface/tuntap/TUNInterface_linux.c \
        interface/tuntap/test/TUNTools.c \
        io/ArrayReader.c \
        io/ArrayWriter.c \
        io/FileReader.c \
        io/FileWriter.c \
        memory/Allocator.c \
        memory/BufferAllocator.c \
        memory/MallocAllocator.c \
        net/ControlHandler.c \
        net/EventEmitter.c \
        net/InterfaceController.c \
        net/NetCore.c \
        net/PeerLink.c \
        net/SessionManager.c \
        net/SwitchPinger.c \
        net/TUNAdapter.c \
        net/UpperDistributor.c \
        switch/EncodingScheme.c \
        switch/SwitchCore.c \
        test/TestFramework.c \
        test/targets/AddrTools_test_wrap.c \
        test/targets/Allocator_test_wrap.c \
        test/targets/AverageRoller_test_wrap.c \
        test/targets/BSDMessageTypeWrapper_test_wrap.c \
        test/targets/Base10_test_wrap.c \
        test/targets/Base32_test_wrap.c \
        test/targets/Beacon_test_wrap.c \
        test/targets/Bits_test_wrap.c \
        test/targets/Checksum_test_wrap.c \
        test/targets/CryptoAddress_test_wrap.c \
        test/targets/CryptoAuth_fuzz_test_wrap.c \
        test/targets/CryptoAuth_test_wrap.c \
        test/targets/CryptoAuth_unit_test_wrap.c \
        test/targets/DHTModules_handleIncoming_test_wrap.c \
        test/targets/DHTModules_handleOutgoing_test_wrap.c \
        test/targets/EncodingScheme_test_wrap.c \
        test/targets/Endian_test_wrap.c \
        test/targets/FileReader_test_wrap.c \
        test/targets/FramingIface_fuzz_test_wrap.c \
        test/targets/Hex_test_wrap.c \
        test/targets/Identity_test_wrap.c \
        test/targets/IpTunnel_test_wrap.c \
        test/targets/JsonBencMessageReader_fuzz_test_wrap.c \
        test/targets/LabelSplicer_test_wrap.c \
        test/targets/Main_fuzz_test_wrap.c \
        test/targets/Map_fuzz_test_wrap.c \
        test/targets/Map_test_wrap.c \
        test/targets/NodeStore_test_wrap.c \
        test/targets/NumberCompress_test_wrap.c \
        test/targets/Process_test_wrap.c \
        test/targets/QSort_test_wrap.c \
        test/targets/Random_test_wrap.c \
        test/targets/ReplayProtector_test_wrap.c \
        test/targets/RouteGen_test_wrap.c \
        test/targets/Seccomp_test_wrap.c \
        test/targets/Set_test_wrap.c \
        test/targets/Sign_test_wrap.c \
        test/targets/Sockaddr_test_wrap.c \
        test/targets/TAPDevice_root_test_wrap.c \
        test/targets/TAPInterface_root_test_wrap.c \
        test/targets/TAPWrapper_root_test_wrap.c \
        test/targets/TUNInterface_ipv4_root_test_wrap.c \
        test/targets/TUNInterface_ipv6_root_test_wrap.c \
        test/targets/TUNInterface_ipv6_withroute_root_test_wrap.c \
        test/targets/VarInt_test_wrap.c \
        test/targets/VersionList_test_wrap.c \
        test/targets/printIp_test_wrap.c \
        test/testcjdroute.c \
        tunnel/IpTunnel.c \
        tunnel/RouteGen.c \
        util/AddrTools.c \
        util/ArrayList.c \
        util/Assert.c \
        util/AverageRoller.c \
        util/Base10.c \
        util/Bits.c \
        util/CString.c \
        util/GlobalConfig.c \
        util/Hex.c \
        util/Order.c \
        util/Pinger.c \
        util/Seccomp_linux.c \
        util/Set.c \
        util/Setuid_linux.c \
        util/events/libuv/EventBase.c \
        util/events/libuv/Pipe.c \
        util/events/libuv/PipeServer.c \
        util/events/libuv/Process.c \
        util/events/libuv/Time.c \
        util/events/libuv/Timeout.c \
        util/events/libuv/UDPAddrIface.c \
        util/log/FileWriterLog.c \
        util/log/Log.c \
        util/log/WriterLog.c \
        util/platform/Sockaddr.c \
        util/platform/netdev/NetDev.c \
        util/platform/netdev/NetPlatform_linux.c \
        util/version/Version.c \
        interface/ETHInterface_linux.c \
        util/ArchInfo.c \
        util/events/libuv/Event.c \
        util/platform/Socket.c \
        wire/Message.c

cjdroute_LDADD = $(NACL_LIBS) $(top_builddir)/libuv/.libs/libuv.a
publictoip6_LDADD = $(NACL_LIBS) $(top_builddir)/libuv/.libs/libuv.a
privatetopublic_LDADD = $(NACL_LIBS) $(top_builddir)/libuv/.libs/libuv.a
sybilsim_LDADD = $(NACL_LIBS) $(top_builddir)/libuv/.libs/libuv.a
makekeys_LDADD = $(NACL_LIBS) $(top_builddir)/libuv/.libs/libuv.a
mkpasswd_LDADD = $(NACL_LIBS) $(top_builddir)/libuv/.libs/libuv.a
randombytes_LDADD = $(NACL_LIBS) $(top_builddir)/libuv/.libs/libuv.a
testcjdroute_LDADD = $(NACL_LIBS) $(top_builddir)/libuv/.libs/libuv.a

# The unit-test wrappers rename the standard main() to <name>_main via the
# preprocessor; the old-style `int main()` definitions cannot be forward
# declared with a real prototype, so relax -Wmissing-prototypes for the
# test program only.
testcjdroute_CFLAGS = $(AM_CFLAGS) -Wno-missing-prototypes

$(top_builddir)/libuv/.libs/libuv.a:
	@if test ! -x $(top_srcdir)/libuv/configure; then \
		cd $(top_srcdir)/libuv && ./autogen.sh; \
	fi
	cd $(top_srcdir)/libuv && ./configure --disable-shared --enable-static \
		CFLAGS="$(CFLAGS) -fPIC"
	$(MAKE) -C $(top_srcdir)/libuv

check-local:
	$(top_builddir)/testcjdroute all

