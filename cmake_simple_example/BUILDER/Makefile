BUILD_DIR=make_build

CXX_INCLUDES = -I../lib
CXXFLAGS = -std=c++11 -O3

all: build_dir logging_example logger

build_dir:
	mkdir $(BUILD_DIR)

logging_example: logger ../app/logging_example.cpp ../lib/logger.h
	g++ $(CXXFLAGS) $(CXX_INCLUDES) -c ../app/logging_example.cpp -o $(BUILD_DIR)/logging_example.o
	g++ $(CXXFLAGS) $(BUILD_DIR)/logging_example.o $(BUILD_DIR)/liblogger.so -o $(BUILD_DIR)/logging_example

logger: ../lib/logger.cpp ../lib/logger.h
	g++ $(CXXFLAGS) -fPIC $(CXX_INCLUDES) -c ../lib/logger.cpp -o $(BUILD_DIR)/logger.o
	g++ $(CXXFLAGS) -fPIC -shared $(BUILD_DIR)/logger.o -o $(BUILD_DIR)/liblogger.so

clean:
	rm -rf $(BUILD_DIR)
