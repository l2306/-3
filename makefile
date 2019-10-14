
INC= -I. -I./comm
INCFLAGS=
CFLAGS=-g -w
LDFLAGS=-lpthread -rdynamic
CC=g++
target= c
src=$(wildcard *.cpp comm/*.cpp)
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

