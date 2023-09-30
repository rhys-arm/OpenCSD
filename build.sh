#!/bin/bash

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR/jopencsd

mv DecodedTraceCallback.h DecodedTraceCallback.h.keep
mv MemAccCallback.h MemAccCallback.h.keep
rm -rf *.h *.cxx *.o *.so bin com
mv DecodedTraceCallback.h.keep DecodedTraceCallback.h
mv MemAccCallback.h.keep MemAccCallback.h

mkdir -p com/arm/debug/trace/decode/jopencsd
SWIG_LIB=swig/swig-4.1.1/Lib  ./swig/swig-4.1.1/swig -java -package com.arm.debug.trace.decode.jopencsd -outdir com/arm/debug/trace/decode/jopencsd -c++ -I../decoder/include -I../decoder/include/opencsd -I../decoder/include/opencsd/c_api -I../decoder/include/common -I../decoder/include/interfaces -I../decoder/include/opencsd/etmv4 -I../decoder/include/opencsd/etmv3 -I../decoder/include/pkt_printers jopencsd.i

DS_JOPENCSD_DIR=/opt/ds/Dev_Arm_DS/work/ds/0.0/source/devstudio/plugins/debugger/com.arm.debug.trace/src/com/arm/debug/trace/decode/jopencsd
rm -rf $DS_JOPENCSD_DIR
mkdir $DS_JOPENCSD_DIR
cp com/arm/debug/trace/decode/jopencsd/* $DS_JOPENCSD_DIR

# build jopencsd
cd $SCRIPT_DIR
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build

# run java program
cd my_app/java_etmv4_c_api
rm -rf bin
javac -Xlint:none -d bin ../../jopencsd/com/arm/debug/trace/decode/jopencsd/*.java runme.java
LD_LIBRARY_PATH=../../build/jopencsd java -cp bin runme
