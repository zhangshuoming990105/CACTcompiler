#!/bin/bash
ANTLR4_RUNTIME="/home/compiler6/runtime"
SRC="src"
GENERATED="grammar"
TARGET="all"

GPP_COMMAND="g++  \
    $SRC/*.cpp\
    $GENERATED/*.cpp \
    -I$SRC/*.h\
    -I$GENERATED\
    -L$ANTLR4_RUNTIME/run/usr/local/lib \
    -lantlr4-runtime \
    -I$ANTLR4_RUNTIME/run/usr/local/include/antlr4-runtime  \
    -I$ANTLR4_RUNTIME/runtime/src \
    -I$ANTLR4_RUNTIME/runtime/src/misc \
    -I$ANTLR4_RUNTIME/runtime/src/atn \
    -I$ANTLR4_RUNTIME/runtime/src/dfa \
    -I$ANTLR4_RUNTIME/runtime/src/tree \
    -I$ANTLR4_RUNTIME/runtime/src/support \
    -std=c++11 \
    -w \
    -o main"
MAKE_COMMAND="make $TARGET"
CP_COMMAND="cp ./build/parser main"

$MAKE_COMMAND
$CP_COMMAND

