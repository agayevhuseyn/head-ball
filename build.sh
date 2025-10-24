#!/bin/bash

CC="gcc"
FLAGS="-Wall -lm -pthread"
OUT="game"

SRC_DIR="src"
OBJ_DIR="objects"
RAYLIB_LIB="raylib-libs"
STATIC=false

usage() {
    echo "unknown flag: $1"
    echo "available: -s|--static"
}

for i in "$@"; do
    case $i in
        -s|--static)
            STATIC=true
            ;;
        *)
            usage $i
            exit 1
            ;;
    esac
done

if $STATIC; then
    FLAGS="$FLAGS $RAYLIB_LIB/linux/libraylib.a
           -I $RAYLIB_LIB/linux/include"
else
    FLAGS="$FLAGS -lraylib"
fi

mkdir -p $OBJ_DIR

for file in $SRC_DIR/*.c; do
	$CC -c $file -I $SRC_DIR -o "$OBJ_DIR/$(basename $file .c).o"
done

$CC $OBJ_DIR/*.o -o $OUT $FLAGS

rm -r $OBJ_DIR
./$OUT
