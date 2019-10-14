
INC= -I. -I../__utils
INCFLAGS=
CFLAGS=-g -Wall
LDFLAGS=-lpthread -rdynamic
CC=g++
target= o
src=$(wildcard *.cpp ../__utils/*.cpp)
cpps=$(notdir $(src))
objs=$(patsubst %.cpp,%.o,$(src))

INST=/usr/local/lib

$(target):$(objs)
	$(CC) $^ $(LDFLAGS) -o $@
%.o:%.cpp
	$(CC) $< -c -o $@

.PHONY:clean
clean:
	-rm -f $(objs)
	
unst:
	rm $(INST)/$(target)
inst:
	cp $(target) $(INST)

mem:
	valgrind --leak-check=yes ./$(target)

