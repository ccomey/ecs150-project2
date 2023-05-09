# Target library
lib := libuthread.a

targets := queue_tester queue_tester_example uthread_hello uthread_yield
objs := queue.o context.o uthread.o preempt.o

CFLAGS := -Wall -Werror

CC := gcc

all: $(lib)

deps := $(patsubst %.o,%.a,$(objs))
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

clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(targets)