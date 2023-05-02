# Target library
lib := libuthread.a

all: $(lib)

## TODO: Phase 1
lib:
	gcc -c -o queue.o queue.c
	ar rcs libuthread.a queue.o
	gcc -o queue_tester queue_tester.c libuthread.a