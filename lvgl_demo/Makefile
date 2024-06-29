#
# Makefile
#
#CC 				?= gcc

CROSS_COMPILE := mips-linux-gnu-

CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

LVGL_DIR_NAME 	?= lvgl
LVGL_DIR 		?= .


SDK_LIB_DIR = ./sdk_uclibc
SDK_INC_DIR = ./sdk_include
INCLUDES = -I$(SDK_INC_DIR)
LIBS = $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a

$(info "-------------------------LIBS start-----------------------------")
$(info $(LIBS))
$(info "-------------------------LIBS end-------------------------------")


WARNINGS		:= -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith \
					-fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess \
					-Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic \
					-Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 \
					-Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter \
					-Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes \
					-Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wno-discarded-qualifiers -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare -std=c99
#CFLAGS 			?= -O3 -g0 -I$(LVGL_DIR)/ $(WARNINGS)

CFLAGS 			?= -O2 -g0 $(INCLUDES) -I$(LVGL_DIR)/ -march=mips32r2 
LDFLAGS 		?= -lm -static

#uclib
CFLAGS += -muclibc
LDFLAGS += -muclibc


LDFLAGS += -Wl,-gc-sections

BIN 			= demo
BUILD_DIR 		= ./build
BUILD_OBJ_DIR 	= $(BUILD_DIR)/obj
BUILD_BIN_DIR 	= $(BUILD_DIR)/bin

prefix 			?= /usr
bindir 			?= $(prefix)/bin

#Collect the files to compile
MAINSRC = 		./main.c

include $(LVGL_DIR)/lvgl/lvgl.mk

#CSRCS 			+=$(LVGL_DIR)/mouse_cursor_icon.c 

OBJEXT 			?= .o

AOBJS 			= $(ASRCS:.S=$(OBJEXT)) #汇编的目标文件
COBJS 			= $(CSRCS:.c=$(OBJEXT)) #C的目标文件

MAINOBJ 		= $(MAINSRC:.c=$(OBJEXT)) #主函数的目标文件

$(info "--------------------- MAINOBJ start ---------------------------")
$(info $(MAINOBJ)) #所有.o文件
$(info "--------------------- MAINOBJ end ---------------------------")

SRCS 			= $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS 			= $(AOBJS) $(COBJS) $(MAINOBJ)

#TARGET 			= $(addprefix $(BUILD_OBJ_DIR)/, $(patsubst ./%, %, $(OBJS)))
TARGET 			= $(patsubst ./%, %, $(OBJS))

$(info "--------------------- TARGET start ---------------------------")
$(info $(TARGET)) #所有.o文件
$(info "--------------------- TARGET end ---------------------------")

## MAINOBJ -> OBJFILES

all: default
#编译，生成所以的.o文件
$(BUILD_OBJ_DIR)/%.o: %.c #目标是.o文件，依赖是.c文件
	@mkdir -p $(dir $@)
	@$(CC)  $(CFLAGS) -c $< -o $@ -lpthread -lm -lrt -ldl -v
	@echo "CC $<"

#链接，把相关的.o文件链接到一起
default: $(TARGET) $(LIBS) sample-common.o
	@mkdir -p $(dir $(BUILD_BIN_DIR)/)
	$(CC) $(LDFLAGS) -o $(BUILD_BIN_DIR)/$(BIN) $(TARGET) sample-common.o $(LIBS) -lpthread -lm -lrt -ldl  
	$(STRIP) $(BUILD_BIN_DIR)/$(BIN)

clean: 
	rm -rf $(BUILD_DIR)
	rm $(TARGET)

install:
	install -d $(DESTDIR)$(bindir)
	install $(BUILD_BIN_DIR)/$(BIN) $(DESTDIR)$(bindir)

uninstall:
	$(RM) -r $(addprefix $(DESTDIR)$(bindir)/,$(BIN))
