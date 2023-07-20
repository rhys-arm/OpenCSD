#!/bin/bash

set -e

mv runme.java runme.java.keep
rm -rf *.java *.cxx *.o *.so bin
mv runme.java.keep runme.java

SWIG_LIB=../swig-4.1.1/Lib  ../swig-4.1.1/swig -java -c++ -I../../../decoder/include -I../../../decoder/include/opencsd -I../../../decoder/include/common -I../../../decoder/include/interfaces -I../../../decoder/include/opencsd/etmv4 -o trc_pkt_types_etmv4_wrap.cxx trc_pkt_types_etmv4.i

g++ -c -fpic -I../../../decoder/include -I../../../decoder/include/opencsd -I../../../decoder/include/common -I../../../decoder/include/interfaces -I../../../decoder/include/opencsd/etmv4 -I"/home/rhys/stuff/jdk-20_linux-x64_bin/jdk-20.0.1/include" -I"/home/rhys/stuff/jdk-20_linux-x64_bin/jdk-20.0.1/include/linux" trc_pkt_types_etmv4_wrap.cxx

g++ -shared -Wl,-z,defs -Wl,-rpath=../../../build/decoder/build/ref_trace_decode_lib -L../../../build/decoder/build/ref_trace_decode_lib trc_pkt_types_etmv4_wrap.o -lopencsd -o libjopencsd.so

echo 1
javac -d bin *.java

echo 2
java -cp bin runme
