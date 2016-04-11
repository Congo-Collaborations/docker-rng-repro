#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1

CC = gcc -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)
#CC = icc -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)

all:  run

run:  run.c tinymt32.o
	${CC} -o $@  run.c tinymt32.o ${LINKOPT}

doc: doxygen.cfg tinymt32.c tinymt32.h mainpage.txt
	doxygen doxygen.cfg

.c.o:
	${CC} -c $<

clean:
	rm -rf run *.o *~ *.dSYM html
