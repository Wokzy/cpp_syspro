CXX ?= g++

all: emulator

emulator : emulator.cpp
	$(CXX) -O2 $^

emulator_sanitized : emulator.cpp
	$(CXX) -O0 -g -fsanitize=address $^

