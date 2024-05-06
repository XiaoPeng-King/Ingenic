/*
 * sample-lcd.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */
#include <stdio.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <sys/mman.h>
#include <unistd.h>
#include <linux/fb.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "sample-common.h"

#define TAG "Sample-LCD"
extern struct chn_conf chn[];

#define FB0DEV			"/dev/fb0"

#define MAX_DESC_NUM    2
#define MAX_LAYER_NUM 	1

/*************************** OSD ***********************************/

#ifdef SUPPORT_RGB555LE
#include "bgramapinfo_rgb555le.h"
#else
//#include "bgramapinfo.h"
#include "bitmapinfo.h"
#endif

#define TAG "Sample-OSD"

#define OSD_LETTER_NUM 20
#define SUPPORT_COLOR_REVERSE

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

/******************************************************************/

struct jzfb_dev {
	unsigned int data_buf[MAX_DESC_NUM][MAX_LAYER_NUM];
	unsigned int num_buf;
	void *buf_addr;

	int width;
	int height;

	unsigned int vid_size;
	unsigned int fb_size;
	int bpp;
	int format;

	struct fb_fix_screeninfo fix_info;
	struct fb_var_screeninfo var_info;

	int fd;
};
// framebuffer 设备初始化
static int jzfb_dev_init(struct jzfb_dev * jzfb_dev)
{
	int ret = 0;
	int i, j;

	//打开设备
	jzfb_dev->fd = open(FB0DEV, O_RDWR);
	if (jzfb_dev->fd <= 2) {
		perror("fb0 open error");
		return jzfb_dev->fd;
	}

	/* get framebuffer's var_info */
	//获得 framebuffer 可变参数信息：
	if ((ret = ioctl(jzfb_dev->fd, FBIOGET_VSCREENINFO, &jzfb_dev->var_info)) < 0) {
		perror("FBIOGET_VSCREENINFO failed");
		goto err_getinfo;
	}

	/* get framebuffer's fix_info */
	//获得 framebuffer 固定信息：分辨率，色深等
	if ((ret = ioctl(jzfb_dev->fd, FBIOGET_FSCREENINFO, &jzfb_dev->fix_info)) < 0) {
		perror("FBIOGET_FSCREENINFO failed");
		goto err_getinfo;
	}

	jzfb_dev->var_info.width = jzfb_dev->var_info.xres;
	jzfb_dev->var_info.height = jzfb_dev->var_info.yres;
	jzfb_dev->bpp = jzfb_dev->var_info.bits_per_pixel >> 3;

	jzfb_dev->width = jzfb_dev->var_info.xres;
	jzfb_dev->height = jzfb_dev->var_info.yres;
	/* format rgb888 use 4 word ; format nv12/nv21 user 2 word */
	jzfb_dev->fb_size = jzfb_dev->var_info.xres * jzfb_dev->var_info.yres * jzfb_dev->bpp;
	jzfb_dev->num_buf = jzfb_dev->var_info.yres_virtual / jzfb_dev->var_info.yres;
	jzfb_dev->vid_size = jzfb_dev->fb_size * jzfb_dev->num_buf;
	//清空 framebuffer 
	jzfb_dev->buf_addr = mmap(0, jzfb_dev->vid_size, PROT_READ | PROT_WRITE, MAP_SHARED, jzfb_dev->fd, 0);
	if(jzfb_dev->buf_addr == 0) {
		perror("Map failed");
		ret = -1;
		goto err_getinfo;
	}

	for(i = 0; i < MAX_DESC_NUM; i++) {
		for(j = 0; j < MAX_LAYER_NUM; j++) {
			jzfb_dev->data_buf[i][j] = (unsigned int)(jzfb_dev->buf_addr +
					j * jzfb_dev->fb_size +
					i * jzfb_dev->fb_size * MAX_LAYER_NUM);
		}
	}

	printf("xres = %d, yres = %d line_length = %d fbsize = %d, num_buf = %d, vidSize = %d\n",
			jzfb_dev->var_info.xres, jzfb_dev->var_info.yres,
			jzfb_dev->fix_info.line_length, jzfb_dev->fb_size,
			jzfb_dev->num_buf, jzfb_dev->vid_size);
	return ret;

err_getinfo:
	close(jzfb_dev->fd);
	return ret;
}

static int jzfb_pan_display(struct jzfb_dev *jzfb_dev, int fram_num)
{
	int ret;
	switch(fram_num) {
	case 0:
		jzfb_dev->var_info.yoffset = jzfb_dev->height * 0;
		break;
	case 1:
		jzfb_dev->var_info.yoffset = jzfb_dev->height * 1;
		break;
	case 2:
		jzfb_dev->var_info.yoffset = jzfb_dev->height * 2;
		break;
	}

	jzfb_dev->var_info.activate = FB_ACTIVATE_NOW;
	ret = ioctl(jzfb_dev->fd, FBIOPAN_DISPLAY, &jzfb_dev->var_info);
	if(ret < 0){
		printf("pan display error!");
		return ret;
	}
	return 0;
}

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
	ret = IMP_OSD_ShowRgn(prHander[5], grpNum, 1);
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
	uint32_t *data = NULL;
	IMPOSDRgnAttrData rAttrData;
	IMPOSDRgnAttr rAttr; //OSD特性参数

	ret = osd_show();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "OSD show error\n");
		return NULL;
	}

	while(1) {
		int penpos = 0;
		int penpos_t = 0;
		int fontadv = 0;
		unsigned int len = 0;

		IMP_OSD_GetRgnAttr(prHander[0], &rAttr);

		data = (void *)rAttr.data.bitmapData;
		time(&currTime);
		currDate = localtime(&currTime);
		memset(DateStr, 0, 40);
		strftime(DateStr, 40, "%Y-%m-%d %I:%M:%S", currDate);
		len = strlen(DateStr);
		for (i = 0; i < len; i++) {
			switch(DateStr[i]) {
				case '0' ... '9':
#ifdef SUPPORT_COLOR_REVERSE
					if(rAttr.fontData.colType[i] == 1) {
						dateData = (void *)gBitmap_black[DateStr[i] - '0'].pdata;
					} else {
						dateData = (void *)gBitmap[DateStr[i] - '0'].pdata;
					}
#else
					dateData = (void *)gBitmap[DateStr[i] - '0'].pdata;
#endif
					fontadv = gBitmap[DateStr[i] - '0'].width;
					penpos_t += gBitmap[DateStr[i] - '0'].width;
					break;
				case '-':
#ifdef SUPPORT_COLOR_REVERSE
					if(rAttr.fontData.colType[i] == 1) {
						dateData = (void *)gBitmap_black[10].pdata;
					} else {
						dateData = (void *)gBitmap[10].pdata;
					}
#else
						dateData = (void *)gBitmap[10].pdata;
#endif
					fontadv = gBitmap[10].width;
					penpos_t += gBitmap[10].width;
					break;
				case ' ':
					dateData = (void *)gBitmap[11].pdata;
					fontadv = gBitmap[11].width;
					penpos_t += gBitmap[11].width;
					break;
				case ':':
#ifdef SUPPORT_COLOR_REVERSE
					if(rAttr.fontData.colType[i] == 1) {
						dateData = (void *)gBitmap_black[12].pdata;
					} else {
						dateData = (void *)gBitmap[12].pdata;
					}
#else
						dateData = (void *)gBitmap[12].pdata;
#endif
					fontadv = gBitmap[12].width;
					penpos_t += gBitmap[12].width;
					break;
				default:
					break;
			}
#ifdef SUPPORT_RGB555LE
			for (j = 0; j < OSD_REGION_HEIGHT; j++) {
				memcpy((void *)((uint16_t *)data + j*OSD_LETTER_NUM*OSD_REGION_WIDTH + penpos_t),
						(void *)((uint16_t *)dateData + j*fontadv), fontadv*sizeof(uint16_t));
			}
#else
			for (j = 0; j < gBitmapHight; j++) {
				memcpy((void *)((intptr_t)data + j*20*32 + penpos),
						(void *)((intptr_t)dateData + j*fontadv), fontadv);
			}
			penpos = penpos_t;
#endif
		}

		sleep(1);
	}

	return NULL;
}

int main(int argc, char *argv[])
{
	int ret,i;
	struct jzfb_dev *jzfb_dev;
	int fram_num = 0;

	IMPFrameInfo *frame_bak;
	IMPFSChnAttr fs_chn_attr[2];

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

	/* Step.0 init jzfb_dev */
	jzfb_dev = calloc(1, sizeof(struct jzfb_dev));
	if (jzfb_dev == NULL) {
		IMP_LOG_ERR(TAG,"jzfb_dev alloc mem for hwc dev failed!");
		return ret;
	}

	ret = jzfb_dev_init(jzfb_dev);
	if(ret) {
		IMP_LOG_ERR(TAG,"jzfb_dev init error!\n");
		return ret;
	}

	/* Step.1 System init */
	ret = sample_system_init();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
		return -1;
	}

	/*Step.2 set sensror fps */
	uint32_t fps_num = 25;
	uint32_t fps_den = 1;
	ret = IMP_ISP_Tuning_SetSensorFPS(0, &fps_num, &fps_den);
	if (ret < 0){
		IMP_LOG_ERR(TAG, "failed to set sensor fps\n");
		return -1;
	}

    
	unsigned char sat_tmp = 0;
    ret = IMP_ISP_Tuning_GetSaturation(0, &sat_tmp);
	sat_tmp = 0;
    ret = IMP_ISP_Tuning_SetSaturation(0, &sat_tmp);


	
	/* Step.3 FrameSource init */
	if(chn[0].enable){
		ret = IMP_FrameSource_CreateChn(chn[0].index, &chn[0].fs_chn_attr);
		if(ret < 0){
			IMP_LOG_ERR(TAG, "IMP_FrameSource_CreateChn(chn%d) error !\n", chn[0].index);
			return -1;
		}
	}

	ret = IMP_FrameSource_SetChnAttr(chn[0].index, &chn[0].fs_chn_attr);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "IMP_FrameSource_SetChnAttr(chn%d) error !\n",  chn[0].index);
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

	IMPCell osdcell = {DEV_ID_OSD, grpNum, 0};
	ret = IMP_System_Bind(&chn[0].framesource_chn, &osdcell);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind FrameSource channel0 and OSD failed\n");
		return -1;
	}

	/* Step.4 set framesource attr config */
	ret = IMP_FrameSource_GetChnAttr(0, &fs_chn_attr[0]);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_GetChnAttr failed\n", __func__, __LINE__);
		return -1;
	}


	fs_chn_attr[0].pixFmt = PIX_FMT_NV12;
	fs_chn_attr[0].i2dattr.i2d_enable = 1;
	fs_chn_attr[0].i2dattr.rotate_enable = 0;
	fs_chn_attr[0].scaler.enable = 1;
	if (fs_chn_attr[0].i2dattr.rotate_enable == 1){
		fs_chn_attr[0].scaler.outwidth = jzfb_dev->height;
		fs_chn_attr[0].scaler.outheight = jzfb_dev->width;
		fs_chn_attr[0].picWidth = jzfb_dev->height;
		fs_chn_attr[0].picHeight = jzfb_dev->width;
		fs_chn_attr[0].i2dattr.rotate_angle = 90;
		}
	else{
		fs_chn_attr[0].scaler.outwidth = jzfb_dev->width;
		fs_chn_attr[0].scaler.outheight = jzfb_dev->height;
		fs_chn_attr[0].picWidth = jzfb_dev->width;
		fs_chn_attr[0].picHeight = jzfb_dev->height;
		}
	
	fs_chn_attr[0].fcrop.enable = 0;
	fs_chn_attr[0].fcrop.left = 160;
	fs_chn_attr[0].fcrop.top = 0;
	fs_chn_attr[0].fcrop.width = 1620;
	fs_chn_attr[0].fcrop.height = 1080;
	

	ret = IMP_FrameSource_SetChnAttr(0, &fs_chn_attr[0]);
	if(ret < 0) {
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetChnAttr failed\n", __func__, __LINE__);
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

	/* Step.5 Stream On 此处有问题*/
	if (chn[0].enable){
		ret = IMP_FrameSource_EnableChn(chn[0].index);

		if(ret < 0){
			IMP_LOG_ERR(TAG, "IMP_FrameSource_EnableChn(%d) error: %d\n", ret, chn[1].index);
			return -1;
		}
	}


	ret = IMP_FrameSource_SetFrameDepth(0, 1);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetFrameDepth failed\n", __func__, __LINE__);
		return -1;
	}

	/* Step.6 Display  */
	while(1) {
		switch(fram_num) {
		case 0:
			ret = IMP_FrameSource_GetFrame(0, &frame_bak);
			if(ret < 0){
				IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_GetFrame failed\n", __func__, __LINE__);
				return -1;
			}
			memcpy((void *)jzfb_dev->data_buf[0][0], (void *)(frame_bak->virAddr), frame_bak->size);
			break;
		case 1:
			ret = IMP_FrameSource_GetFrame(0, &frame_bak);
			if(ret < 0){
				IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_GetFrame failed\n", __func__, __LINE__);
				return -1;
			}
			memcpy((void *)jzfb_dev->data_buf[1][0], (void *)(frame_bak->virAddr), frame_bak->size);
			break;
		default:
			break;
		}
		if (ret = jzfb_pan_display(jzfb_dev, fram_num))
			break;
		fram_num ++;
		if (fram_num > 1)
			fram_num = 0;

		IMP_FrameSource_ReleaseFrame(0, frame_bak);
	}

	pthread_cancel(tid);
	pthread_join(tid, NULL);
	free(timeStampData);

	ret = IMP_FrameSource_SetFrameDepth(0, 0);
	if(ret < 0) {
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetFrameDepth failed\n", __func__, __LINE__);
		return -1;
	}


	/* Step.7 Stream Off */
	if (chn[0].enable) {
		ret = IMP_FrameSource_DisableChn(chn[0].index);
		if(ret < 0){
			IMP_LOG_ERR(TAG, "IMP_FrameSource_DisableChn(%d) error: %d\n", ret, chn[0].index);
			return -1;
		}
	}

	/* Step.8 FrameSource exit */
	if (chn[0].enable){
		ret = IMP_FrameSource_DestroyChn(0);
		if(ret < 0){
			IMP_LOG_ERR(TAG, "IMP_FrameSource_DestroyChn() error: %d\n", ret);
			return -1;
		}
	}

	/* Step.9 System exit */
	ret = sample_system_exit();
	if(ret < 0){
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	munmap(jzfb_dev->buf_addr, jzfb_dev->vid_size);
	close(jzfb_dev->fd);
	free(jzfb_dev);

	return 0;

}
