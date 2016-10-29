TARGET := $(lastword $(subst /, ,$(CURDIR)))
ifneq (,$(WINDIR))
    TARGET := $(TARGET).exe
endif

SOURCES := $(shell ls *.c 2>/dev/null)

CC = gcc
CFLAGS = -std=c90 -ansi -pedantic-errors -Wall -Werror -O2 -g

.PHONY: all clean

ifneq (,$(SOURCES))
all: $(TARGET)
$(TARGET): $(SOURCES)
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@
else
all:
	@echo No source files found.
endif

clean:
	$(RM) *.o $(TARGET)
