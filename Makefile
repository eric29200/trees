CFLAGS  := -Wall -Wextra -O2 -g
LDFLAGS := $(shell pkg-config --libs gtk+-3.0) -lm
CC      := gcc

all: main

main: tree.o binary_tree.o avl_tree.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.o: .c
	$(CC) $(CFLAGS) -c $^

clean :
	rm -f *.o */*.o main 
