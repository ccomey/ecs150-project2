# Target library
lib := libuthread.a

targets := queue_tester queue_tester_example uthread_hello uthread_yield sem_simple sem_count sem_buffer sem_prime
objs := queue.o context.o uthread.o preempt.o sem.o

CFLAGS := -Wall -Werror

CC := gcc

APPS := apps
APPSPATH := ../$(APPS)
libuthread := $(APPSPATH)/$(APPS).a

## Include path
CFLAGS 	+= -I$(APPSPATH)

## Dependency generation
CFLAGS	+= -MMD

all: $(lib)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

ifneq ($(V), 1)
Q = @
endif

## TODO: Phase 1
%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

libuthread.a: $(objs)
	$(Q)ar rcs $@ $^

queue_tester: $(lib)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ ../apps/queue_tester.c $<

queue_tester_example: $(lib)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ ../apps/queue_tester_example.c $<

uthread_hello: $(lib)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ ../apps/uthread_hello.c $<

uthread_yield: $(lib)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ ../apps/uthread_yield.c $<

sem_simple: $(lib)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ ../apps/sem_simple.c $<

sem_count: $(lib)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ ../apps/sem_count.c $<

sem_buffer: $(lib)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ ../apps/sem_buffer.c $<

sem_prime: $(lib)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ ../apps/sem_prime.c $<

clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(targets) *.d