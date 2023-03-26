# Linux compiler
BCC=gcc

# Windows compiler
#BCC=x86_64-w64-mingw32-gcc

#CC=$(BCC)
CC=$(BCC) -Wall -Wno-unknown-pragmas

.PHONY = dirguard clean

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=%.o)
BINDIR = out

mysh: $(OBJS)
	@echo Linking final executable $@
	@${CC} -g $(OBJS:%=$(BINDIR)/%) -o $@

dirguard:
	@mkdir -p $(BINDIR)

%.o: %.c %.h defines.h dirguard
	@echo Compiling $@
	@${CC} -g -c $< -o $(BINDIR)/$@

clean:
	@echo Removing compiled binaries
	@rm -rf $(BINDIR)/ mysh
