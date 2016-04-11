#for GNU make

#DDEBUG = -O0 -g -ggdb -DDEBUG=1

CC = gcc -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)
#CC = icc -Wall -O3 -std=c99 -Wmissing-prototypes $(DDEBUG)

all:  run

run:  run.c mt64.o
	${CC} -o $@  run.c mt64.o ${LINKOPT}

.c.o:
	${CC} -c $<

clean:
	rm -rf run *.o *~