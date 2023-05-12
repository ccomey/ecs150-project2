# Target library
lib := libuthread.a

objs := queue.o context.o uthread.o preempt.o sem.o

CFLAGS := -Wall -Wextra -Werror

CC := gcc

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

clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(targets) *.d