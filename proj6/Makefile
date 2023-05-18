# Linux compiler
BCC=gcc

# Windows compiler
#BCC=x86_64-w64-mingw32-gcc

#CC=$(BCC)
CC=$(BCC) -Wall

.PHONY = clean testdirs

SRCS := $(wildcard *.c)
TEST_SRCS := $(wildcard testing/*/*.c)

BINDIR = out
TEST_DIRS := $(wildcard testing/*/)
TEST_BINDIRS := $(TEST_DIRS:%=%bin/)

OBJS := $(SRCS:%.c=$(BINDIR)/%.o)
TEST_OBJS := $(TEST_SRCS:%.c=%.out)

EMPTY := $() $()

all: thread tests

thread: $(OBJS)
	@echo Linking final executable $@
	@${CC} -g $(OBJS) -o $@

tests: testdirs $(TEST_OBJS)

testing/%.out: testing/%.c mthread_lib.c mthread_lib.h
# Hacky work around for splicing in the directory for test binaries
	$(eval TEMP := $(subst /, ,$@))
	$(eval FILE := $(lastword $(TEMP)))
	$(eval DIR := $(subst $(EMPTY),/,$(filter-out $(FILE), $(TEMP))/bin))
	@echo Compiling test $<
	@${CC} -g -I $(shell pwd) mthread_lib.c mthread_sem.c mthread_message.c $< -o $(DIR)/$(FILE)

${BINDIR}/%.o: %.c %.h
	@mkdir -p $(BINDIR)
	@echo Compiling $@
	@${CC} -g -c $< -o $@

testdirs:
	@mkdir -p $(TEST_BINDIRS)

clean:
	@echo Removing compiled binaries
	@rm -rf $(BINDIR)/ $(TEST_BINDIRS) thread
