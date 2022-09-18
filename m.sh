#!/bin/sh
cd "$(dirname "$0")"
set -ex

case "$1" in
"s")
    rm -rf "build" && mkdir "build"
    cmake -G "Ninja" \
        -DCMAKE_BUILD_TYPE="Release" \
        -DCMAKE_EXE_LINKER_FLAGS="-static" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
        -S "." -B "build"
    cmake --build "build" --parallel
    ls -lh "build/src/imageconv"
    ;;
"r")
    rm -rf "build" && mkdir "build"
    cmake -G "Ninja" \
        -DCMAKE_BUILD_TYPE="Release" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
        -S "." -B "build"
    cmake --build "build" --parallel
    ls -lh "build/src/imageconv"
    ;;
"p")
    rm -rf "build" && mkdir "build"
    cmake -G "Ninja" \
        -DCMAKE_BUILD_TYPE="RelWithDebInfo" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
        -S "." -B "build"
    cmake --build "build" --parallel
    ls -lh "build/src/imageconv"
    ;;
*)
    # default to debug builds
    # ln -sf build/compile_commands.json compile_commands.json
    mkdir -p "build"
    cmake -G "Ninja" \
        -DCMAKE_BUILD_TYPE="Debug" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
        -S "." -B "build"
    cmake --build "build" --parallel
    ls -lh "build/src/imageconv"
    (cd build/src/tests && ctest)
    ;;
esac
