CONFIG_UCLIBC_BUILD=y

CROSS_COMPILE = mips-linux-gnu-

CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip


INCLUDES += -I. -I./inc
TARGET_NAME = ND03_Ranging_Example

C_SRCS	= $(wildcard *.c) src/nd03_calib.c src/nd03_comm.c src/nd03_data.c src/nd03_dev.c
C_OBJS = $(patsubst %.c, %.o, $(C_SRCS))

$(warning $(C_OBJS))

CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2

ifeq ($(CONFIG_UCLIBC_BUILD), y)
CFLAGS += -muclibc
LDFLAG += -muclibc
endif

LDFLAG += -Wl,-gc-sections

all:$(TARGET_NAME)

$(TARGET_NAME) : $(C_OBJS)
	$(CC) $(LDFLAG) -o $@ $(C_OBJS) -lm -lpthread -lrt -ldl

$(C_OBJS) : %.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
