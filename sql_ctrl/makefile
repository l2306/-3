# Makefile Module For Develop Team

.SUFFIXES:
.SUFFIXES:  .c .o

WORKDIR=.

LIBDIR=$(HOME)/tmplib

CC=gcc

libsql_ctrl.so: *.h *.c
	$(CC) -shared -fPIC $^ -I/usr/include/ -L/usr/lib64/mysql  -lmysqlclient   -lm -lrt -ldl  -lstdc++ -lpthread -o $@
	@cp $@ $(LIBDIR)

	
.PHONY : clean
clean :
	rm  -f *.o
	rm  -f *.s
	rm  -f *.so
