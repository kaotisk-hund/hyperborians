#!/bin/sh
# Usage: ./do [configure options...]
# Runs autoreconf, configure, then make. Wrapper for the automake build.
set -e

autoreconf -i

[ -n "${CJDNS_CC:-}" ] && CC_OPT="CC=$CJDNS_CC" || CC_OPT=""
[ -n "${CJDNS_CXX:-}" ] && CXX_OPT="CXX=$CJDNS_CXX" || CXX_OPT=""

./configure "$@"
exec make -j"${CJDNS_JOBS:-$(getconf _NPROCESSORS_ONLN)}" $CC_OPT $CXX_OPT