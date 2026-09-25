AC_PREREQ([2.69])
AC_INIT([cjdns], [21.0])
AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_SRCDIR([crypto/CryptoAuth.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([foreign subdir-objects])
LT_INIT
AC_PROG_CC

#
# Platform detection.
# Certain sources in the cjdns tree rely on platform macros (cjdns historically
# passed -Dlinux=1 etc. on the command line); define the same so the code
# compiles as before.  These become the -D flags below.
#
case "$host" in
    *android*)          CJDNS_SYSTEM=android;  CJDNS_SYSDEF=android ;;
    *linux*)            CJDNS_SYSTEM=linux;    CJDNS_SYSDEF=linux ;;
    *darwin*)           CJDNS_SYSTEM=darwin;   CJDNS_SYSDEF=darwin ;;
    *freebsd*)          CJDNS_SYSTEM=freebsd;  CJDNS_SYSDEF=freebsd ;;
    *openbsd*)          CJDNS_SYSTEM=openbsd;  CJDNS_SYSDEF=openbsd ;;
    *solaris*|*sunos*)  CJDNS_SYSTEM=sunos;    CJDNS_SYSDEF=sunos ;;
    *mingw*|*msys*|*windows*) CJDNS_SYSTEM=win32; CJDNS_SYSDEF=win32 ;;
    *)                  CJDNS_SYSTEM=unknown;  CJDNS_SYSDEF=UNKNOWN ;;
esac

AC_ARG_ENABLE([kcp], [AS_HELP_STRING([--enable-kcp], [Cryptographic primitives (no-op placeholder)])], [], [])

AC_ARG_ENABLE([log-level],
    [AS_HELP_STRING([--enable-log-level=DEBUG], [Log level: KEYS, DEBUG, INFO, WARN, ERROR, CRITICAL])],
    [cjdns_log_level=$enableval],
    [cjdns_log_level=DEBUG])
case "$cjdns_log_level" in
    KEYS|DEBUG|INFO|WARN|ERROR|CRITICAL) ;;
    *) AC_MSG_ERROR([invalid log level '$cjdns_log_level' (use KEYS, DEBUG, INFO, WARN, ERROR or CRITICAL)]) ;;
esac

AC_ARG_ENABLE([pie],
    [AS_HELP_STRING([--disable-pie], [Disable position independent executables])],
    [cjdns_pie=$enableval],
    [if test "x$CJDNS_SYSTEM" = "xfreebsd" -o "x$CJDNS_SYSTEM" = "xwin32"; then
         cjdns_pie=no
     else
         cjdns_pie=yes
     fi])

AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug], [Build with -O0 and full debug info])],
    [cjdns_debug=$enableval],
    [cjdns_debug=no])

AC_ARG_ENABLE([identity-check],
    [AS_HELP_STRING([--disable-identity-check], [Disable Identity_CHECK safety checking])],
    [cjdns_identity=$enableval],
    [cjdns_identity=yes])

AC_ARG_ENABLE([allocator-canaries],
    [AS_HELP_STRING([--disable-allocator-canaries], [Disable Allocator_USE_CANARIES])],
    [cjdns_canaries=$enableval],
    [cjdns_canaries=yes])

AC_ARG_WITH([number-compress-type],
    [AS_HELP_STRING([--with-number-compress-type=TYPE], [NumberCompress size encoding: v3x5x8, v4x8, f4 or f8])],
    [CJDNS_CPPFLAGS="$CJDNS_CPPFLAGS -DNumberCompress_TYPE=$withval"],
    [CJDNS_CPPFLAGS="$CJDNS_CPPFLAGS -DNumberCompress_TYPE=v3x5x8"])

AC_ARG_WITH([address-prefix],
    [AS_HELP_STRING([--with-address-prefix=BITS], [Maximum address prefix length (ADDRESS_PREFIX_BITS)])], 
    [CJDNS_CPPFLAGS="$CJDNS_CPPFLAGS -DADDRESS_PREFIX_BITS=$withval"], [])

#
# Compiler / linker flags
#
if test "x$cjdns_debug" = "xyes"; then
    CJDNS_OPTFLAGS="-O0 -g"
else
    CJDNS_OPTFLAGS="-O3 -g"
    CJDNS_CPPFLAGS="$CJDNS_CPPFLAGS -D_FORTIFY_SOURCE=2"
fi

if test "x$cjdns_pie" = "xyes"; then
    CJDNS_CFLAGS_SPLIT="$CJDNS_CFLAGS_SPLIT -fPIE"
    if test "x$CJDNS_SYSTEM" = "xdarwin"; then
        CJDNS_PIE_LDFLAGS="-Wl,-pie"
    else
        CJDNS_PIE_LDFLAGS="-pie"
    fi
fi

CJDNS_CPPFLAGS="$CJDNS_CPPFLAGS -D${CJDNS_SYSDEF}=1 -DLog_${cjdns_log_level}"

if test "x$cjdns_identity" = "xyes"; then
    CJDNS_CPPFLAGS="$CJDNS_CPPFLAGS -DIdentity_CHECK=1"
fi
if test "x$cjdns_canaries" = "xyes"; then
    CJDNS_CPPFLAGS="$CJDNS_CPPFLAGS -DAllocator_USE_CANARIES=1"
fi

# Enable by default so unused parameters don't error (cjdns has many)
CJDNS_CPPFLAGS="$CJDNS_CPPFLAGS -DPARANOIA=1"

case "$CJDNS_SYSTEM" in
    linux|darwin) CJDNS_CPPFLAGS="$CJDNS_CPPFLAGS -DHAS_ETH_INTERFACE=1" ;;
esac

# cjdns lets the compiler know its own version
CJDNS_CPPFLAGS="$CJDNS_CPPFLAGS -DCJD_PACKAGE_VERSION=\\\"$PACKAGE_VERSION\\\""

# Libraries historically needed for libuv on linux
AC_SEARCH_LIBS([clock_gettime], [rt])
AC_SEARCH_LIBS([dlopen], [dl])
AC_SEARCH_LIBS([pthread_create], [pthread])

AC_SUBST([CJDNS_SYSTEM])
AC_SUBST([CJDNS_SYSDEF])
AC_SUBST([CJDNS_CPPFLAGS])
AC_SUBST([CJDNS_OPTFLAGS])
AC_SUBST([CJDNS_PIE_LDFLAGS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT