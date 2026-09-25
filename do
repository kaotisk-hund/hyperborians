#!/bin/sh
# Usage: ./do [configure options...]
# Runs autoreconf, configure, then make. Wrapper for the automake build.
set -e

autoreconf -i

[ -n "${HYPERBORIA_CC:-}" ] && CC_OPT="CC=$HYPERBORIA_CC" || CC_OPT=""
[ -n "${HYPERBORIA_CXX:-}" ] && CXX_OPT="CXX=$HYPERBORIA_CXX" || CXX_OPT=""

./configure "$@"
exec make -j"${HYPERBORIA_JOBS:-$(getconf _NPROCESSORS_ONLN)}" $CC_OPT $CXX_OPT