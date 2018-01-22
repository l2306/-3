
INCFLAGS=-I./inc
CFLAGS=-g -w -shared
LDFLAGS=-lpthread
CC=g++
target=libThrdPool.so
src=$(wildcard ./src/*.c)
objs=$(patsubst %.c,%.o,$(src))

VPATH = ./src

INST=/usr/local/lib

$(target):$(objs)
	$(CC) $^ -shared $(LDFLAGS) -o $@
%.o:%.c
	$(CC) $< -c -fpic -o $@

.PHONY:clean
clean:
	-rm -f $(objs) $(target)
unst:
	rm $(INST)/$(target)
inst:
	cp $(target) $(INST)

