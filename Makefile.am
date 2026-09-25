AUTOMAKE_OPTIONS = foreign subdir-objects
ACLOCAL_AMFLAGS = -I m4

# ---- compilation model --------------------------------------------------
#
# cjdns passes its own set of flags; the platform/logging/safety defines all
# get folded into AM_CPPFLAGS via configure substitution.
AM_CPPFLAGS = -I$(top_srcdir) $(CJDNS_CPPFLAGS)

AM_CFLAGS = $(CJDNS_OPTFLAGS) -std=c99 -Wall -Wextra -Werror \
	-Wno-pointer-sign -Wmissing-prototypes -pedantic -Wno-unused-parameter \
	-fomit-frame-pointer

AM_LDFLAGS = $(CJDNS_PIE_LDFLAGS)

# ---- vendored libuv ------------------------------------------------------
.PHONY: libuv
libuv:
	@if test ! -x $(top_srcdir)/libuv/configure; then \
		cd $(top_srcdir)/libuv && ./autogen.sh; \
	fi
	cd $(top_srcdir)/libuv && ./configure --disable-shared --enable-static
	$(MAKE) -C $(top_srcdir)/libuv

# ---- nacl: pruned djb NaCl ref implementations --------------------------
#
# The ref sources are written against "abstract" symbol names (crypto_box(),
# crypto_stream(), ...).  Each convenience library compiles them with -D
# renames so the abstract names resolve to the concrete primitive symbols
# that the rest of cjdns actually calls.  No -Werror here: this is vendored
# public-domain code, tolerate whatever the current compiler says.
NACL_CFLAGS = $(CJDNS_OPTFLAGS) -std=c99 -Wall -Wextra -Wno-pointer-sign \
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