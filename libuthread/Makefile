# Target library
lib := libuthread.a

all: $(lib)

## TODO: Phase 1
queue.o: queue.c queue.h
	gcc -c -o queue.o queue.c

preempt.o: preempt.c private.h uthread.h
	gcc -c -o preempt.o preempt.c

uthread.o: uthread.c uthread.h queue.h private.h
	gcc -c -o uthread.o uthread.c

context.o: context.c private.h uthread.h
	gcc -c -o context.o context.c

libuthread.a: queue.o uthread.o context.o preempt.o
	ar rcs libuthread.a queue.o uthread.o preempt.o context.o 

queue_tester:
	gcc -c -o queue.o queue.c
	ar rcs libuthread.a queue.o
	gcc -o queue_tester ../apps/queue_tester.c libuthread.a

queue_tester_example:
	gcc -c -o queue.o queue.c
	ar rcs libuthread.a queue.o
	gcc -o queue_tester_example ../apps/queue_tester_example.c libuthread.a

uthread_hello: libuthread.a
	gcc -o uthread_hello ../apps/uthread_hello.c libuthread.a

clean:
	rm -f *.a *.o queue_tester queue_tester_example uthread_hello