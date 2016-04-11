#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1

CC = gcc -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)
#CC = icc -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)

all:  run

run:  run.c tinymt64.o
	${CC} -o $@  run.c tinymt64.o ${LINKOPT}

doc: doxygen.cfg tinymt64.c tinymt64.h mainpage.txt
	doxygen doxygen.cfg

.c.o:
	${CC} -c $<

clean:
	rm -rf run *.o *~ *.dSYM html
