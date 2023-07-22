#!/bin/bash

set -e

mv runme.java runme.java.keep
mv Callback.h Callback.h.keep
rm -rf *.java *.h *.cxx *.o *.so bin
mv runme.java.keep runme.java
mv Callback.h.keep Callback.h

SWIG_LIB=../java/swig-4.1.1/Lib  ../java/swig-4.1.1/swig -java -c++ -I../../decoder/include -I../../decoder/include/opencsd -I../../decoder/include/opencsd/c_api -I../../decoder/include/common -I../../decoder/include/interfaces -I../../decoder/include/opencsd/etmv4 -I../../decoder/include/pkt_printers jopencsd.i

echo 1
# TODO remove -fpermissive
g++ -c -fpermissive -fpic -I../../decoder/include -I../../decoder/include/opencsd -I../../decoder/include/opencsd/c_api -I../../decoder/include/common -I../../decoder/include/interfaces -I../../decoder/include/opencsd/etmv4 -I../../decoder/include/pkt_printers -I"/home/rhys/stuff/jdk-20_linux-x64_bin/jdk-20.0.1/include" -I"/home/rhys/stuff/jdk-20_linux-x64_bin/jdk-20.0.1/include/linux" jopencsd_wrap.cxx

echo 2
g++ -shared -Wl,-z,defs -Wl,-rpath=../../build/decoder/build/rctdl_c_api_lib -Wl,-rpath=../../build/decoder/build/ref_trace_decode_lib -L../../build/decoder/build/rctdl_c_api_lib -L../../build/decoder/build/ref_trace_decode_lib jopencsd_wrap.o -lopencsd_c_api -lopencsd -o libjopencsd.so

echo 3
# TODO remove use of -Xlint
javac -Xlint:none -d bin *.java
# javac -d bin *.java

echo 4
LD_LIBRARY_PATH=. java -cp bin runme
