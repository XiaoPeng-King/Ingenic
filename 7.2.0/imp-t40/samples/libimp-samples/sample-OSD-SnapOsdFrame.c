/*
 * sample-OSD-SnapOsdFrame.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_osd.h>
#include <imp/imp_utils.h>

#include "sample-common.h"

#ifdef SUPPORT_RGB555LE
#include "bgramapinfo_rgb555le.h"
#else
#include "bgramapinfo.h"
//#include "bitmapinfo.h"
#endif

#define TAG "Sample-OSD"

#define OSD_LETTER_NUM 20

extern struct chn_conf chn[];
static int byGetFd = 0;

int grpNum = 0;
IMPRgnHandle *prHander;

#if RESIZE_TIMESTAMP
static uint32_t re_num0_bgra[16*34*4*4] = {0};
static uint32_t re_num1_bgra[16*34*4*4] = {0};
static uint32_t re_num2_bgra[16*34*4*4] = {0};
static uint32_t re_num3_bgra[16*34*4*4] = {0};
static uint32_t re_num4_bgra[16*34*4*4] = {0};
static uint32_t re_num5_bgra[16*34*4*4] = {0};
static uint32_t re_num6_bgra[16*34*4*4] = {0};
static uint32_t re_num7_bgra[16*34*4*4] = {0};
static uint32_t re_num8_bgra[16*34*4*4] = {0};
static uint32_t re_num9_bgra[16*34*4*4] = {0};
static uint32_t re_minus_bgra[16*34*4*4] = {0};
static uint32_t re_blank_bgra[16*34*4*4] = {0};
static uint32_t re_colon_bgra[16*34*4*4] = {0};

bitmapinfo_t re_gBgramap[13] = {
	{
		.width = 16*2,
		.pdata = re_num0_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_num1_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_num2_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_num3_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_num4_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_num5_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_num6_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_num7_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_num8_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_num9_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_minus_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_blank_bgra,
	},
	{
		.width = 16*2,
		.pdata = re_colon_bgra,
	},
};
#endif

static int osd_show(void)
{
	int ret;

	ret = IMP_OSD_ShowRgn(prHander[0], grpNum, 1);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() timeStamp error\n");
		return -1;
	}
	ret = IMP_OSD_ShowRgn(prHander[1], grpNum, 1);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Logo error\n");
		return -1;
	}
	ret = IMP_OSD_ShowRgn(prHander[2], grpNum, 1);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
		return -1;
	}
	ret = IMP_OSD_ShowRgn(prHander[3], grpNum, 1);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Rect error\n");
		return -1;
	}
	ret = IMP_OSD_ShowRgn(prHander[4], grpNum, 1);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Line error\n");
		return -1;
	}

	return 0;
}

static void *update_thread(void *p)
{
	int ret;

	/*generate time*/
	char DateStr[40];
	time_t currTime;
	struct tm *currDate;
	unsigned i = 0, j = 0;
	void *dateData = NULL;
	uint32_t *data = p;
	IMPOSDRgnAttrData rAttrData;

	ret = osd_show();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "OSD show error\n");
		return NULL;
	}

	while(1) {
		int penpos_t = 0;
		int fontadv = 0;

		time(&currTime);
		currDate = localtime(&currTime);
		memset(DateStr, 0, 40);
		strftime(DateStr, 40, "%Y-%m-%d %I:%M:%S", currDate);
		for (i = 0; i < OSD_LETTER_NUM; i++) {
#if RESIZE_TIMESTAMP
			switch(DateStr[i]) {
				case '0' ... '9':
					dateData = (void *)re_gBgramap[DateStr[i] - '0'].pdata;
					fontadv = re_gBgramap[DateStr[i] - '0'].width;
					penpos_t += re_gBgramap[DateStr[i] - '0'].width;
					break;
				case '-':
					dateData = (void *)re_gBgramap[10].pdata;
					fontadv = re_gBgramap[10].width;
					penpos_t += re_gBgramap[10].width;
					break;
				case ' ':
					dateData = (void *)re_gBgramap[11].pdata;
					fontadv = re_gBgramap[11].width;
					penpos_t += re_gBgramap[11].width;
					break;
				case ':':
					dateData = (void *)re_gBgramap[12].pdata;
					fontadv = re_gBgramap[12].width;
					penpos_t += re_gBgramap[12].width;
					break;
				default:
					break;
			}
#else
			switch(DateStr[i]) {
				case '0' ... '9':
					dateData = (void *)gBgramap[DateStr[i] - '0'].pdata;
					fontadv = gBgramap[DateStr[i] - '0'].width;
					penpos_t += gBgramap[DateStr[i] - '0'].width;
					break;
				case '-':
					dateData = (void *)gBgramap[10].pdata;
					fontadv = gBgramap[10].width;
					penpos_t += gBgramap[10].width;
					break;
				case ' ':
					dateData = (void *)gBgramap[11].pdata;
					fontadv = gBgramap[11].width;
					penpos_t += gBgramap[11].width;
					break;
				case ':':
					dateData = (void *)gBgramap[12].pdata;
					fontadv = gBgramap[12].width;
					penpos_t += gBgramap[12].width;
					break;
				default:
					break;
			}
#endif

#ifdef SUPPORT_RGB555LE
			for (j = 0; j < OSD_REGION_HEIGHT; j++) {
				memcpy((void *)((uint16_t *)data + j*OSD_LETTER_NUM*OSD_REGION_WIDTH + penpos_t),
						(void *)((uint16_t *)dateData + j*fontadv), fontadv*sizeof(uint16_t));
			}
#else
			for (j = 0; j < OSD_REGION_HEIGHT; j++) {
				memcpy((void *)((uint32_t *)data + j*OSD_LETTER_NUM*OSD_REGION_WIDTH + penpos_t),
						(void *)((uint32_t *)dateData + j*fontadv), fontadv*sizeof(uint32_t));
			}

#endif
		}
		rAttrData.picData.pData = data;
		IMP_OSD_UpdateRgnAttrData(prHander[0], &rAttrData);

		sleep(1);
	}

	return NULL;
}

OSD_Frame_CALLBACK callback(int group_index, IMPFrameInfo *frame) {
	/* !!!Don't use blocking code in callback functions!!!
	If you need to use the framedata for a long time, please memcpy to the extra memory*/
	char nv12_name[20];
	sprintf(nv12_name, "/tmp/osdsnap%d.nv12", group_index);
	int osdsnapfd = open(nv12_name, O_RDWR | O_CREAT | O_TRUNC);
	if (osdsnapfd >= 0) {
		write(osdsnapfd, (void *)frame->virAddr, frame->width * frame->height);
		write(osdsnapfd, (void *)frame->virAddr + frame->width * ((frame->height + 15) & ~15),
			frame->width * frame->height / 2);
		close(osdsnapfd);
	}
}

int main(int argc, char *argv[])
{
	int i, ret;

	if (argc >= 2) {
		byGetFd = atoi(argv[1]);
	}

	/* only show OSD in chn0 */
	chn[0].enable = 1;
	chn[1].enable = 0;
	chn[2].enable = 0;
	chn[3].enable = 0;

#if RESIZE_TIMESTAMP
	OpsParam osd_param;
	osd_param.inputParam.input_batch = 1;
	osd_param.inputParam.input_width = 16;
	osd_param.inputParam.input_height = 34;
	osd_param.inputParam.input_channel = 4;
	osd_param.outputParam.output_width = 32;
	osd_param.outputParam.output_height = 68;
	for(i=0;i<13;i++){
		IMP_OSD_ResizeRGB_Pic((uint8_t *)gBgramap[i].pdata, (uint8_t *)re_gBgramap[i].pdata, false, &osd_param);
	}
#endif

	/* Step.1 System init */
	ret = sample_system_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
		return -1;
	}
	/* Step.2 FrameSource init */
	ret = sample_framesource_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource init failed\n");
		return -1;
	}

	/* Step.3 Encoder init */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_Encoder_CreateGroup(chn[i].index);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", chn[i].index);
				return -1;
			}
		}
	}

	ret = sample_encoder_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder init failed\n");
		return -1;
	}

	/* Step.4 OSD init */
	if (IMP_OSD_CreateGroup(grpNum) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_CreateGroup(%d) error !\n", grpNum);
		return -1;
	}

	prHander = sample_osd_init(grpNum);
	if (prHander <= 0) {
		IMP_LOG_ERR(TAG, "OSD init failed\n");
		return -1;
	}

	/* Step.5 Bind */
	IMPCell osdcell = {DEV_ID_OSD, grpNum, 0};
	ret = IMP_System_Bind(&chn[0].framesource_chn, &osdcell);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind FrameSource channel0 and OSD failed\n");
		return -1;
	}

	ret = IMP_System_Bind(&osdcell, &chn[0].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind OSD and Encoder failed\n");
		return -1;
	}

	/* Step.6 Create OSD bgramap update thread */
	pthread_t tid;
#ifdef SUPPORT_RGB555LE
	uint32_t *timeStampData = malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint16_t));
#else
	uint32_t *timeStampData = malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint32_t));
#endif
	if (timeStampData == NULL) {
		IMP_LOG_ERR(TAG, "valloc timeStampData error\n");
		return -1;
	}
	memset(timeStampData, 0, OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint32_t));

	ret = pthread_create(&tid, NULL, update_thread, timeStampData);
	if (ret) {
		IMP_LOG_ERR(TAG, "thread create error\n");
		return -1;
	}

	/* Called after IMP_OSD_CreateGroup and before IMP_FrameSource_EnableChn */
	IMP_OSD_SetGroupCallback(0, (void *)callback);

	/* Step.7 Stream On */
	IMP_FrameSource_SetFrameDepth(0, 0);
	ret = sample_framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}

	/* Step.6 Get stream */
	if (byGetFd) {
		ret = sample_get_video_stream_byfd();
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "Get video stream byfd failed\n");
			return -1;
		}
	} else {
		ret = sample_get_video_stream();
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "Get video stream failed\n");
			return -1;
		}
	}

	/* Exit sequence as follow */
	/* Step.a Stream Off */
	ret = sample_framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
		return -1;
	}

	pthread_cancel(tid);
	pthread_join(tid, NULL);
	free(timeStampData);

	/* Step.b UnBind */
	ret = IMP_System_UnBind(&osdcell, &chn[0].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind OSD and Encoder failed\n");
		return -1;
	}

	ret = IMP_System_UnBind(&chn[0].framesource_chn, &osdcell);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "UnBind FrameSource and OSD failed\n");
		return -1;
	}

	/* Step.c OSD exit */
	ret = sample_osd_exit(prHander,grpNum);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "OSD exit failed\n");
		return -1;
	}

	/* Step.d Encoder exit */
	ret = sample_encoder_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder exit failed\n");
		return -1;
	}

	/* Step.e FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}

	/* Step.f System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}
