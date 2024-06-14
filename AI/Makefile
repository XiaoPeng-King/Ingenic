TOPDIR ?= ./
CROSS_COMPILE:=mips-linux-gnu-

CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)g++
STRIP = $(CROSS_COMPILE)strip
libtype = muclibc
build_type = release
#build_type ?= profile
#build_type ?= debug
#build_type ?= nmem

CXXFLAGS := -std=c++11 -mfp64 -mnan=2008 -mabs=2008 -Wall -EL -O3 -march=mips32r2 -flax-vector-conversions -lpthread -lrt -ldl -lm
INCLUDES := -I 

ifeq ($(libtype), muclibc)
	CXXFLAGS += -muclibc
endif

TARGET = IAIrisClientSample
LIBS := -L 

OBJS := IAIrisClientSample.o


%.o:%.cpp
	$(CPP) $(INCLUDES) $(CXXFLAGS) -o $@ -c $^

$(TARGET):$(OBJS)
	$(CPP) $(CXXFLAGS) $(OBJS) -o $@ $(INCLUDES) $(LIBS)

all:$(TARGET)

.PHONY: clean
clean: 
	rm -f $(TARGET) $(OBJS)
