#/bin/bash

mv test.cpp2 test.cpp

g++ *.cpp ../__comm/*.cpp -I../__comm -g -std=c++11 -lpthread -rdynamic -o test.out

mv test.cpp test.cpp2

valgrind --leak-check=yes ./a.out
