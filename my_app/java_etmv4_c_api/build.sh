#!/bin/bash

set -e

# ensure opencsd lib is up to date
here=$(pwd)
cd ../..
cmake --build build
cd $here

mv Callback.h Callback.h.keep
mv Callback2.h Callback2.h.keep
rm -rf *.h *.cxx *.o *.so bin com
mv Callback.h.keep Callback.h
mv Callback2.h.keep Callback2.h

mkdir -p com/arm/debug/trace/decode/jopencsd
SWIG_LIB=../java/swig-4.1.1/Lib  ../java/swig-4.1.1/swig -java -package com.arm.debug.trace.decode.jopencsd -outdir com/arm/debug/trace/decode/jopencsd -c++ -I../../decoder/include -I../../decoder/include/opencsd -I../../decoder/include/opencsd/c_api -I../../decoder/include/common -I../../decoder/include/interfaces -I../../decoder/include/opencsd/etmv4 -I../../decoder/include/opencsd/etmv3 -I../../decoder/include/pkt_printers jopencsd.i

DS_JOPENCSD_DIR=/opt/ds/Dev_Arm_DS/work/ds/0.0/source/devstudio/plugins/debugger/com.arm.debug.trace/src/com/arm/debug/trace/decode/jopencsd
if [ -d "$DS_JOPENCSD_DIR" ]; then
    cp /opt/temp/OpenCSD/my_app/java_etmv4_c_api/com/arm/debug/trace/decode/jopencsd/* $DS_JOPENCSD_DIR
else
    echo "$DS_JOPENCSD_DIR does not exist, so the generated .java files will not be copied to it"
fi

echo 1
# TODO remove -fpermissive
g++ -c -fpermissive -fpic -I../../decoder/include -I../../decoder/include/opencsd -I../../decoder/include/opencsd/c_api -I../../decoder/include/common -I../../decoder/include/interfaces -I../../decoder/include/opencsd/etmv4 -I../../decoder/include/opencsd/etmv3 -I../../decoder/include/pkt_printers -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/linux" jopencsd_wrap.cxx

echo 2
g++ -shared -Wl,-z,defs -Wl,-rpath=../../build/decoder/build/rctdl_c_api_lib -Wl,-rpath=../../build/decoder/build/ref_trace_decode_lib -L../../build/decoder/build/rctdl_c_api_lib -L../../build/decoder/build/ref_trace_decode_lib jopencsd_wrap.o -lopencsd_c_api -lopencsd -o libjopencsd.so

echo 3
# TODO remove use of -Xlint
javac -Xlint:none -d bin com/arm/debug/trace/decode/jopencsd/*.java runme.java
# javac -d bin *.java

echo 4
LD_LIBRARY_PATH=. java -cp bin runme
