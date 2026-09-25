#!/usr/bin/env bash

# http://hyperboria.ca/hyperboria-droid.sh

# Does most things required to build hyperboria for android.
# See bottom of file for tips on installing/usage.
# ADB, Android, plus basic command line skills required
# ircerr 20140507

# Parts stolen from:
#  https://github.com/cjdelisle/hyperboria/pull/476
#  https://gist.github.com/lgierth/01ce4bda638f8c863349
#  larsg@HypeIRC
# + mods by prurigro

# Update files from:
#  https://developer.android.com/tools/sdk/ndk/index.html

# Possible Deps (phone):
#  1. rooted:
#   The method required to root a phone differs from model to model.
#   If your phone isn't rooted yet and you're not sure where to
#   start, look for the subforum for your device on XDA forums
#   (linked below), and hopefully you'll find something that works.
#
#   http://forum.xda-developers.com/index.php?tab=all
#
#  2. tun device:
#   Most (if not all) 4.0+ phones include tun support. If yours
#   uses 2.x, CyanogenMod and some stock ROMs include support, but
#   many don't. If your phone doesn't have a TUN device at /dev/tun,
#   download the link below (or find 'com.aed.tun.installer' to
#   download it yourself), then install and run it to have it set
#   one up for you.
#
#   http://hyperboria.ca/com.aed.tun.installer.apk

# Report success/failure including phone type, android version, kernel version,
# and as much information as possible to #hyperboria @ HypeIRC

# NOTES:
#  Use a custom NDK directory:
#   Before running this script, configure $NDK: export NDK="/path/to/ndk"
#
#  Use a different repo:
#   Remove 'hyperboria-android/hyperboria' and below change: hyperboria_repo="https://newaddr"
#
#  Use a different branch:
#   Run: cjdroid-bulid.sh branchname

##CONFIGURABLE VARIABLES
hyperboria_repo="https://github.com/cjdelisle/hyperboria/"
[[ -n "$1" ]] \
    && hyperboria_repo_branch="-$1"

build_dir="$PWD/build_android"
src_dir="$build_dir/source"
ndk_dir="$src_dir/ndk"
work_dir="$build_dir/workspace"

ndkver="android-ndk-r10e"
cpu_arch="$(uname -m)"
[[ -z "$cpu_arch" ]] && {
    echo "ERROR: NO CPU ARCHITECTURE DETECTED"
    exit 1
}
[[ "$cpu_arch" = "i686" ]] \
    && cpu_arch="x86"

##CREATE REQUIRED DIRECTORIES
install -d "$src_dir"
install -d "$work_dir"

##SETUP NDK
cd "$src_dir"
[[ -z "$NDK" ]] && {
    if [ -z "$ANDROID_NDK" ]; then
        echo "$ndkver-linux-${cpu_arch}.bin"
        [[ -f "$ndkver-linux-${cpu_arch}.bin" ]] \
            || wget "http://dl.google.com/android/ndk/$ndkver-linux-${cpu_arch}.bin" \
            || (echo "Can't find download for your system" && exit 1)
        [[ -d "$ndkver" ]] || (chmod a+x $ndkver-linux-${cpu_arch}.bin && ./$ndkver-linux-${cpu_arch}.bin || exit 1)
        NDK="$ndkver"
    else
        NDK="$ANDROID_NDK"
    fi
}
[[ ! -d "$NDK" ]] && {
    echo "The NDK variable is not pointing to a valid directory"
    exit 1
}
[[ -h "$ndk_dir" ]] \
    && rm "$ndk_dir"
[[ ! -e "$ndk_dir" ]] \
    && ln -sf "$NDK" "$ndk_dir"

GCC=$work_dir/android-arm-toolchain/bin/arm-linux-androideabi-gcc
TOOLCHAIN=arm-linux-androideabi-4.9
COMPILER=arm-linux-androideabi-
[[ "x$TARGET_ARCH" == "xarm64" ]] \
    && GCC=$work_dir/android-arm-toolchain/bin/aarch64-linux-android-gcc \
    && TOOLCHAIN=aarch64-linux-android-4.9 \
    && COMPILER=aarch64-linux-android-


##BUILD TOOLCHAIN: build gcc toolchain
[[ ! -x "$GCC" ]] && {
    cd "$src_dir"
    "$ndk_dir/build/tools/make-standalone-toolchain.sh" \
        --platform=android-21 \
        --toolchain=$TOOLCHAIN \
        --install-dir="$work_dir/android-arm-toolchain/" \
        --system=linux-$cpu_arch \
            || exit 1
}

##CLONE or PULL: the repo and change branch if requested
cd "$build_dir"
[[ -d hyperboria ]] && {
    cd hyperboria
    git pull --ff-only
} || {
    git clone $hyperboria_repo hyperboria
    [[ ! -d hyperboria ]] && {
        echo "ERROR: Couldn't clone $hyperboria_repo"
        exit 1
    }
    cd hyperboria
}
[[ -n "$1" ]] \
    && git checkout "$1"
./clean

##SETUP TOOLCHAIN VARS
export PATH="$work_dir/android-arm-toolchain/bin:$PATH"

##BUILD hyperboria (without tests)
CROSS_COMPILE=$COMPILER ./cross-do 2>&1 \
    | tee hyperboria-build.log
[[ ! -f 'hyperboria-route' ]] && {
    echo -e "\nBUILD FAILED :("
    exit 1
}
echo -e "\nBUILD COMPLETE! @ $build_dir/hyperboria/hyperboria-route"

##PACKAGE CJDROUTE AND ASSOCIATED SCRIPTS FOR DEPLOYMENT
cd "$build_dir"
hyperboria_version=$(git -C hyperboria describe --always | sed 's|-|.|g;s|[^\.]*\.||;s|\.[^\.]*$||')
[[ -f ../cjdroid-$hyperboria_version${hyperboria_repo_branch}.tar.gz ]] && {
    echo "Error: Package not built because $(readlink -f ../cjdroid-$hyperboria_version${hyperboria_repo_branch}.tar.gz) already exists"
    exit 1
}
[[ ! -f hyperboria/hyperboria-route ]] && {
    echo "Error: Package not built because $PWD/hyperboria/hyperboria-route does not exist"
    exit 1
}
[[ ! -d hyperboria/contrib/android/cjdroid ]] && {
    echo "Error: Package not built because $PWD/hyperboria/contrib/android/cjdroid does not exist"
    exit 1
}
cp -R hyperboria/contrib/android/cjdroid .
install -Dm755 hyperboria/hyperboria-route cjdroid/files/hyperboria-route
tar cfz ../cjdroid-$hyperboria_version${hyperboria_repo_branch}.tar.gz cjdroid
echo -e "\nSuccess: A deployable package has been created @ $(readlink -f ../cjdroid-$hyperboria_version${hyperboria_repo_branch}.tar.gz)"
