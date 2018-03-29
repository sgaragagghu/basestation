CC = gcc
CFLAGS = -g -Wall -Wextra -O2
SRC = basestation.o utils.o list.o rngs.o rvgs.o
OBJ = $(SRC:.c=.o)
LIBS ?= #-lpthread
LDFLAGS = -lm

all: $(OBJ) 
	${CC} ${CFLAGS} basestation.o utils.o list.o rngs.o rvgs.o $(LIBS) -o basestation $(LDFLAGS)

basestation.o:basestation.h
utils.o:utils.h
list.o:list.h
rngs.o:rngs.h
rvgs.o:rvgs.h





clean:
	rm -f *.o core 

cleanall:
	rm -f *.o core basestation
