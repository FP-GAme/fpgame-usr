# The name of the application to be built.
TARGET = con_test

# The architecture being compiled for.
ARCH = arm

# The objects to be compiled from .c and .S source files
COBJ = $(patsubst %.c,%.o,$(shell find . -name '*.c'))
ASMOBJ =

# The folders to include headers from, reletive to make
INC = src/inc usr/inc

# Libraries to be linked to the binary.
LIBS = -Lusr/ -lfpgame

# The compiler to be used and its C flags.
CC = arm-none-linux-gnueabihf-gcc
CFLAGS = -std=c99
