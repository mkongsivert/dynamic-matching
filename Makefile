CXX = clang++
CC = clang
CXX_FLAGS = -g -std=c++11 -pedantic -Wall -Wextra -I. -I/home/mackenzie/include -L/home/mackenzie/lib

matching: matching.cpp matching.hpp
	$(CXX) matching.cpp -o matching $(CXX_FLAGS) -lncurses

clean:
	rm -rf *.o matching