# Linux compiler
BCC=gcc

# Windows compiler
#BCC=x86_64-w64-mingw32-gcc

#CC=$(BCC)
CC=$(BCC) -Wall -Wno-unknown-pragmas -pthread

.PHONY = clean

SRCS := $(wildcard *.c)
BINDIR = out
OBJS := $(SRCS:%.c=$(BINDIR)/%.o)

mysh: $(OBJS)
	@echo Linking final executable $@
	@${CC} -g $(OBJS) -o $@

${BINDIR}/%.o: %.c %.h defines.h
	@mkdir -p $(BINDIR)
	@echo Compiling $@
	@${CC} -g -c $< -o $@

clean:
	@echo Removing compiled binaries
	@rm -rf $(BINDIR)/ mysh
