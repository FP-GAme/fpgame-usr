# The name of the application to be built.
TARGET = apu_clean

# The architecture being compiled for.
ARCH = arm

# The objects to be compiled from .c and .S source files
COBJ = $(patsubst %.c,%.o,$(shell find . -name '*.c'))
ASMOBJ =

# Objects to be created from binary files.
BINS = bins/clean.o

# The folders to include headers from, reletive to make
-include sdk.mk
override INC += src/inc usr/inc

# Libraries to be linked to the binary.
LIBS = -Lusr/ -lfpgame

# The compiler to be used and its C flags.
CC = arm-none-linux-gnueabihf-gcc
LD = arm-none-linux-gnueabihf-ld
CFLAGS = -nostdinc -std=c99
