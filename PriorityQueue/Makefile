CC=gcc
CFLAGS=-I. -g3
DEPS = PriorityQueue.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: PriorityQueue.o PriorityQueueMain.o 
	$(CC) -o PriorityQueueMain PriorityQueue.o PriorityQueueMain.o

.PHONY: clean

clean:
	rm -f *.o PriorityQueueMain
