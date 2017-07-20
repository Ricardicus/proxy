CFLAGS = -Wall -lpthread

all:
	gcc src/*.c $(CFLAGS) -o backend 
