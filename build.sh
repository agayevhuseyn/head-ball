#!/bin/bash

CC="gcc"
FLAGS="-Wall -lm -pthread"
OUT="game"

SRC_DIR="src"
OBJ_DIR="objects"
RAYLIB_LIB="raylib-libs"
STATIC=false
PLATFORM="linux"

usage() {
    echo "unknown flag: $1"
    echo "available: -s|--static --windows"
}

for i in "$@"; do
    case $i in
        -s|--static)
            STATIC=true
            ;;
        --windows)
            PLATFORM="win"
            STATIC=true
            ;;
        *)
            usage $i
            exit 1
            ;;
    esac
done

if $STATIC; then
    LINKED="$RAYLIB_LIB/$PLATFORM/libraylib.a"
    echo "Statically linking with $LINKED ..."
    FLAGS="$FLAGS $LINKED"
fi
if [[ $PLATFORM == "linux" ]]; then
    echo "Compiling for Linux..."
    if ! $STATIC; then
        FLAGS="$FLAGS -ldl -lX11 -lraylib"
    fi
else
    echo "Compiling for Windows..."
    FLAGS="$FLAGS -lgdi32 -lwinmm"
fi

if [[ $PLATFORM == "win" ]]; then
    CC="x86_64-w64-mingw32-gcc"
    OUT="$OUT.exe"
    if ! command -v $CC &>/dev/null; then
        echo "$CC compiler is not found on your machine"
        exit 1
    fi
fi

mkdir -p $OBJ_DIR

for file in $SRC_DIR/*.c; do
	$CC -c $file -I $SRC_DIR -o "$OBJ_DIR/$(basename $file .c).o" -I $RAYLIB_LIB/$PLATFORM/include
done

$CC $OBJ_DIR/*.o -o $OUT $FLAGS

echo "Removing object files..."
rm -r $OBJ_DIR

echo "Executable is ready as $OUT"
