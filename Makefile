CXX ?= g++

mini_2 : mini_2.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_3 : mini_3_1.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_4 : mini_4.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_6 : mini_6.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_9 : mini_9.cpp
	$(CXX) -O0 -g -fsanitize=address $^
