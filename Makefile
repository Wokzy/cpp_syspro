CXX ?= g++

mini_2 : mini_2.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_3 : mini_3_1.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_4 : mini_4.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_6 : mini_6.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_8 : mini_8.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_9 : mini_9.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_10 : mini_10.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_11 : mini_11.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_12 : mini_12.cpp
	$(CXX) -O0 -g -fsanitize=address $^

mini_13 : mini_13.cpp
	$(CXX) -O2 -fsanitize=address $^
