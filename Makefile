CXX ?= g++

mini_2 : mini_2.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_3 : mini_3_1.cpp
	$(CXX) -O0 -g -fsanitize=address $^
