AC_PREREQ([2.69])
AC_INIT([hyperboriaNS], [1.0.0])
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
# Certain sources in the hyperboria tree rely on platform macros (hyperboria historically
# passed -Dlinux=1 etc. on the command line); define the same so the code
# compiles as before.  These become the -D flags below.
#
case "$host" in
    *android*)          HYPERBORIA_SYSTEM=android;  HYPERBORIA_SYSDEF=android ;;
    *linux*)            HYPERBORIA_SYSTEM=linux;    HYPERBORIA_SYSDEF=linux ;;
    *darwin*)           HYPERBORIA_SYSTEM=darwin;   HYPERBORIA_SYSDEF=darwin ;;
    *freebsd*)          HYPERBORIA_SYSTEM=freebsd;  HYPERBORIA_SYSDEF=freebsd ;;
    *openbsd*)          HYPERBORIA_SYSTEM=openbsd;  HYPERBORIA_SYSDEF=openbsd ;;
    *solaris*|*sunos*)  HYPERBORIA_SYSTEM=sunos;    HYPERBORIA_SYSDEF=sunos ;;
    *mingw*|*msys*|*windows*) HYPERBORIA_SYSTEM=win32; HYPERBORIA_SYSDEF=win32 ;;
    *)                  HYPERBORIA_SYSTEM=unknown;  HYPERBORIA_SYSDEF=UNKNOWN ;;
esac

AC_ARG_ENABLE([kcp], [AS_HELP_STRING([--enable-kcp], [Cryptographic primitives (no-op placeholder)])], [], [])

AC_ARG_ENABLE([log-level],
    [AS_HELP_STRING([--enable-log-level=DEBUG], [Log level: KEYS, DEBUG, INFO, WARN, ERROR, CRITICAL])],
    [hyperboria_log_level=$enableval],
    [hyperboria_log_level=DEBUG])
case "$hyperboria_log_level" in
    KEYS|DEBUG|INFO|WARN|ERROR|CRITICAL) ;;
    *) AC_MSG_ERROR([invalid log level '$hyperboria_log_level' (use KEYS, DEBUG, INFO, WARN, ERROR or CRITICAL)]) ;;
esac

AC_ARG_ENABLE([pie],
    [AS_HELP_STRING([--disable-pie], [Disable position independent executables])],
    [hyperboria_pie=$enableval],
    [if test "x$HYPERBORIA_SYSTEM" = "xfreebsd" -o "x$HYPERBORIA_SYSTEM" = "xwin32"; then
         hyperboria_pie=no
     else
         hyperboria_pie=yes
     fi])

AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug], [Build with -O0 and full debug info])],
    [hyperboria_debug=$enableval],
    [hyperboria_debug=no])

AC_ARG_ENABLE([identity-check],
    [AS_HELP_STRING([--disable-identity-check], [Disable Identity_CHECK safety checking])],
    [hyperboria_identity=$enableval],
    [hyperboria_identity=yes])

AC_ARG_ENABLE([allocator-canaries],
    [AS_HELP_STRING([--disable-allocator-canaries], [Disable Allocator_USE_CANARIES])],
    [hyperboria_canaries=$enableval],
    [hyperboria_canaries=yes])

AC_ARG_WITH([number-compress-type],
    [AS_HELP_STRING([--with-number-compress-type=TYPE], [NumberCompress size encoding: v3x5x8, v4x8, f4 or f8])],
    [HYPERBORIA_CPPFLAGS="$HYPERBORIA_CPPFLAGS -DNumberCompress_TYPE=$withval"],
    [HYPERBORIA_CPPFLAGS="$HYPERBORIA_CPPFLAGS -DNumberCompress_TYPE=v3x5x8"])

AC_ARG_WITH([address-prefix],
    [AS_HELP_STRING([--with-address-prefix=BITS], [Maximum address prefix length (ADDRESS_PREFIX_BITS)])], 
    [HYPERBORIA_CPPFLAGS="$HYPERBORIA_CPPFLAGS -DADDRESS_PREFIX_BITS=$withval"], [])

#
# Compiler / linker flags
#
if test "x$hyperboria_debug" = "xyes"; then
    HYPERBORIA_OPTFLAGS="-O0 -g"
else
    HYPERBORIA_OPTFLAGS="-O3 -g"
    HYPERBORIA_CPPFLAGS="$HYPERBORIA_CPPFLAGS -D_FORTIFY_SOURCE=2"
fi

if test "x$hyperboria_pie" = "xyes"; then
    HYPERBORIA_CFLAGS_SPLIT="$HYPERBORIA_CFLAGS_SPLIT -fPIE"
    if test "x$HYPERBORIA_SYSTEM" = "xdarwin"; then
        HYPERBORIA_PIE_LDFLAGS="-Wl,-pie"
    else
        HYPERBORIA_PIE_LDFLAGS="-pie"
    fi
fi

HYPERBORIA_CPPFLAGS="$HYPERBORIA_CPPFLAGS -D${HYPERBORIA_SYSDEF}=1 -DLog_${hyperboria_log_level}"

if test "x$hyperboria_identity" = "xyes"; then
    HYPERBORIA_CPPFLAGS="$HYPERBORIA_CPPFLAGS -DIdentity_CHECK=1"
fi
if test "x$hyperboria_canaries" = "xyes"; then
    HYPERBORIA_CPPFLAGS="$HYPERBORIA_CPPFLAGS -DAllocator_USE_CANARIES=1"
fi

# Enable by default so unused parameters don't error (hyperboria has many)
HYPERBORIA_CPPFLAGS="$HYPERBORIA_CPPFLAGS -DPARANOIA=1"

case "$HYPERBORIA_SYSTEM" in
    linux|darwin) HYPERBORIA_CPPFLAGS="$HYPERBORIA_CPPFLAGS -DHAS_ETH_INTERFACE=1" ;;
esac

# hyperboria lets the compiler know its own version
HYPERBORIA_CPPFLAGS="$HYPERBORIA_CPPFLAGS -DHYPERBORIA_PACKAGE_VERSION=\\\"$PACKAGE_VERSION\\\""

# Some compilers (gcc 16+) warn that Allocator_calloc(alloc, sizeof(x), 1)
# looks like calloc(nmemb, size) with the arguments transposed.  The order is
# deliberate and matches every call site in the tree, so silence the warning
# on compilers that understand it (others must not be told about it).
AC_CACHE_CHECK([for -Wno-calloc-transposed-args], [hyperboria_cv_wcalloc_transposed], [
    AC_LANG_PUSH([C])
    HYPERBORIA_SAVE_CFLAGS=$CFLAGS
    CFLAGS="$CFLAGS -Werror -Wno-calloc-transposed-args"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[ ]], [[return 0;]])],
        [hyperboria_cv_wcalloc_transposed=yes],
        [hyperboria_cv_wcalloc_transposed=no])
    CFLAGS=$HYPERBORIA_SAVE_CFLAGS
    AC_LANG_POP([C])
])
if test "x$hyperboria_cv_wcalloc_transposed" = "xyes"; then
    HYPERBORIA_WARN_FLAGS="$HYPERBORIA_WARN_FLAGS -Wno-calloc-transposed-args"
fi

# Libraries historically needed for libuv on linux
AC_SEARCH_LIBS([clock_gettime], [rt])
AC_SEARCH_LIBS([dlopen], [dl])
AC_SEARCH_LIBS([pthread_create], [pthread])

AC_SUBST([HYPERBORIA_SYSTEM])
AC_SUBST([HYPERBORIA_SYSDEF])
AC_SUBST([HYPERBORIA_CPPFLAGS])
AC_SUBST([HYPERBORIA_OPTFLAGS])
AC_SUBST([HYPERBORIA_CFLAGS_SPLIT])
AC_SUBST([HYPERBORIA_PIE_LDFLAGS])

AC_SUBST([HYPERBORIA_WARN_FLAGS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT