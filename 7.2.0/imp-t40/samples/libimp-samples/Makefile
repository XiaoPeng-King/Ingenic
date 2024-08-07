CONFIG_UCLIBC_BUILD=y

#CROSS_COMPILE ?= mips-linux-gnu-
CROSS_COMPILE = mips-linux-gnu-

CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2

ifeq ($(CONFIG_UCLIBC_BUILD), y)
CFLAGS += -muclibc
LDFLAG += -muclibc
endif

ifeq ($(CONFIG_UCLIBC_BUILD), y)
SDK_LIB_DIR = ../../lib/uclibc
else
SDK_LIB_DIR = ../../lib/glibc
endif

SDK_INC_DIR = ../../include

INCLUDES = -I$(SDK_INC_DIR)

LIBS = $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a

LDFLAG += -Wl,-gc-sections

SAMPLES = sample-Encoder-video \
	sample-OSD \
	sample-Audio \
	sample-Ai \
	sample-Ao \
	sample-Ai-AEC \
	sample-Ai-Ref \
	sample-dmic \
	sample-dmic-aec \
	sample-dmic-ref \
	sample-Setfps \
	sample-ISP-flip \
	sample-Encoder-jpeg \
	sample-Encoder-yuv \
	sample-Encoder-yuvprocess \
	sample-Encoder-h264-jpeg \
	sample-Encoder-video-IVS-move \
	sample-Encoder-h264-IVS-base-move\
	sample-Change-Resolution \
	sample-Snap-Raw \
	sample-Snap-YUV \
	sample-Encoder-video-IVS-exalgo \
	sample-Framesource \
	sample-LCD \
	sample-Framesource-algo \
	sample-AutoZoom \
	sample-Fcrop \
	sample-IVS-unbind-move \
	sample-Encoder-video-AddSeiUDU \
	sample-IspOsd	\
	sample-OSD-SnapOsdFrame  \
	sample-I2d	\
	sample-GetFrameEx \
	sample-Encoder-Joint \
	sample-Encoder-video-change-dualsensor \
	sample-Encoder-video-pip \
	fb_test \
	sample-LCD-Display \

all: 	$(SAMPLES)

sample-Ai: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Ai.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ao: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Ao.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ai-AEC: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Ai-AEC.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Ai-Ref: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Ai-Ref.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-dmic: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-dmic.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-dmic-aec: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-dmic-aec.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-dmic-ref: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-dmic-ref.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Encoder-video: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Encoder-video.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-video-AddSeiUDU: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Encoder-video-AddSeiUDU.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-jpeg: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Encoder-jpeg.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-yuv: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-Encoder-yuv.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-yuvprocess: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-Encoder-yuvprocess.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-h264-jpeg: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Encoder-h264-jpeg.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-OSD: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-OSD.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Audio: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Audio.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt -ldl
	$(STRIP) $@

sample-Setfps: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Setfps.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-ISP-flip: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-ISP-flip.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-video-IVS-move: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Encoder-video-IVS-move.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-h264-IVS-base-move: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Encoder-h264-IVS-base-move.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-IVS-unbind-move: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-IVS-unbind-move.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Change-Resolution: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Change-Resolution.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Snap-Raw: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Snap-Raw.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Snap-YUV: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Snap-YUV.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-video-IVS-exalgo: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Encoder-video-IVS-exalgo.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Framesource: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Framesource.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-LCD: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-LCD.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Framesource-algo: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Framesource-algo.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-AutoZoom: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-AutoZoom.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Fcrop: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Fcrop.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-IspOsd: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-IspOsd.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-OSD-SnapOsdFrame: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-OSD-SnapOsdFrame.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-I2d: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-I2d.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-GetFrameEx: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-GetFrameEx.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-Joint: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Encoder-Joint.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-video-change-dualsensor: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Encoder-video-change-dualsensor.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-Encoder-video-pip: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-Encoder-video-pip.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

fb_test: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o fb_test.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

sample-LCD-Display: $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a sample-common.o sample-LCD-Display.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

%.o:%.c sample-common.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
