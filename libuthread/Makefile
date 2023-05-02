# Target library
lib := libuthread.a

all: $(lib)

## TODO: Phase 1
lib:
	gcc -c -o queue.o queue.c
	ar rcs libuthread.a queue.o

queue_tester:
	gcc -c -o queue.o queue.c
	ar rcs libuthread.a queue.o
	gcc -o queue_tester ../apps/queue_tester.c libuthread.a

queue_tester_example:
	gcc -c -o queue.o queue.c
	ar rcs libuthread.a queue.o
	gcc -o queue_tester_example ../apps/queue_tester_example.c libuthread.a

clean:
	rm -f *.a *.o queue_tester queue_tester_example