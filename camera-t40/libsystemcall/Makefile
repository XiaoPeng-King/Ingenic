CROSS_COMPILE ?= mips-linux-gnu-

CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

ifeq ($(CROSS_COMPILE),mips-linux-gnu-)
CFLAGS += -muclibc
endif

DBUS_COMMON_PATH=./dbus/
DBUS_COMMON_INCLUDE=$(DBUS_COMMON_PATH)

OBJS_DAEMON = ./system_call_daemon.o
OBJS_CLIENT = ./system_call.o
INCLUDE = -I./include/ -I$(DBUS_COMMON_INCLUDE) $(DBUS_INCLUDE)
CFLAGS += -lpthread $(INCLUDE) -fPIC

all: libsystem_call system_call_daemon system_call_test

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.cpp
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@

libsystem_call: $(OBJS_CLIENT)
	$(CC) --share -o $@.so $(OBJS_CLIENT) $(CFLAGS) ./dbus/uclibc/libdbus.a
	$(AR) $@.a $(OBJS_CLIENT)

system_call_daemon:$(OBJS_DAEMON)
	$(CC) -o $@ $(OBJS_DAEMON) $(LIBS) $(CFLAGS) ./dbus/uclibc/libdbus.a
	$(STRIP) $@

system_call_test: libsystem_call.a system_call_test.o
	$(CC) -o $@ system_call_test.o libsystem_call.a $(CFLAGS) ./dbus/uclibc/libdbus.a
	$(STRIP) $@

clean:
	rm $(OBJS_DAEMON) $(OBJS_CLIENT) -f
	rm libsystem_call.a -f
	rm libsystem_call.so -f
	rm system_call_daemon -f
	rm system_call_test -f
	rm *.o -f

distclean: clean
