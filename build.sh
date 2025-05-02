#!/bin/bash

CC="gcc"
FLAGS="-Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11"
OUT="game"

SRC_DIR="src"
OBJ_DIR="objects"

mkdir -p $OBJ_DIR

for file in $SRC_DIR/*.c; do
	$CC -c $file -I $SRC_DIR -o "$OBJ_DIR/$(basename $file .c).o" $FLAGS
done

$CC $OBJ_DIR/*.o -o $OUT $FLAGS

rm -r $OBJ_DIR
./$OUT
