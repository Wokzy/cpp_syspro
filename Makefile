CXX ?= g++

mini_2 : mini_2.cpp
	$(CXX) -O0 -g -fsanitize=address $^
