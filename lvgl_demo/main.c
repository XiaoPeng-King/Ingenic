#include "lvgl/lvgl.h"
//#include "lvgl/demos/lv_demos.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "sdk_include/imp/imp_log.h"
#include "sdk_include/imp/imp_common.h"
#include "sdk_include/imp/imp_system.h"
#include "sdk_include/imp/imp_framesource.h"
#include "sdk_include/imp/imp_encoder.h"

#include <sys/mman.h>
#include <unistd.h>
#include <linux/fb.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <errno.h>

#include "./sample-common.h"

#define TAG "Sample-LVGL"
extern struct chn_conf chn[];

uint8_t img_lvgl_rgb888_map[320*480*3] = {};

const lv_img_dsc_t img_lvgl_test = {
  .header.magic = LV_IMAGE_HEADER_MAGIC,
  .header.cf = LV_COLOR_FORMAT_RGB888,
  .header.flags = 0,
  .header.w = 320,
  .header.h = 480,
  .header.stride = 320 * 3,
  .data_size = 320*480*3,
  .data = img_lvgl_rgb888_map,
};

bool g_swap_img_buffer_flag = 0;

#define FB0DEV			"/dev/fb0"

#define MAX_DESC_NUM    2
#define MAX_LAYER_NUM 	1

#if 0
void NV12toRGB888(int width, int height, uint8_t *nv12_data, uint8_t *rgb888)
{
	int frameSize = width * height;
    int y, u, v;
    int r, g, b;
    int index = 0;
 
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            y = (int)nv12_data[i * width + j];
            u = (int)nv12_data[frameSize + (i / 2) * width + j / 2 * 2];
            v = (int)nv12_data[frameSize + (i / 2) * width + j / 2 * 2 + 1];
 
            int v_r = v - 128;
            r = y + v_r + ((v_r * 103) >> 8);
            r = range_limit(r);
 
            int u_g = u - 128;
            int v_g = v - 128;
            g = y + ((u_g * 88) >> 8) - ((v_g * 183) >> 8);
            g = range_limit(g);
 
            u_g = u - 128;
            b = y + u_g + ((u_g * 198) >> 8);
            b = range_limit(b);
 
 
            QRgb pixelValue = qRgb(r, g, b);
 
            //设置像素值
            qimage->setPixel(j, i, pixelValue);
 
        }
    }
}
#endif

//unsigned char rgb_buf[320*480*3]={};

/**
 * NV12属于YUV420SP格式
 * @param data
 * @param rgb
 * @param width
 * @param height
 */

char Offset_Y = 16;
char Offset_U = 128;
char Offset_V = 128;

void NV12_TO_RGB24(unsigned char *data, unsigned char *rgb, int width, int height) {
    int index = 0;
    unsigned char *ybase = data;
    unsigned char *ubase = &data[width * height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            //YYYYYYYYUVUV
            unsigned char Y = ybase[x + y * width];
            unsigned char U = ubase[y / 2 * width + (x / 2) * 2];
            unsigned char V = ubase[y / 2 * width + (x / 2) * 2 + 1];

			//y = 128; uv = 127
			//R = 255 + 1.772 * (127 - 128) = 253
			//G = 255 - 0.34413 * (127 - 128) - 0.71414 * (127 - 128) = 256
            //rgb[index++] = Y + 1.772 * (U - 128); //R
            //rgb[index++] = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128); //G
            //rgb[index++] = Y + 1.402 * (V - 128); //B

			//rgb[index++] = Y + (140 * (V -128))/100;  //r
			//rgb[index++] = Y - (34 * (U-128))/100 - (71 * (V-128))/100; //g
			//rgb[index++] = Y + (177 * (U-128))/100; //b

			//1.164 * (128 - 16) + 2.018 * (127 - 128) = 128

			//rgb[index++] = 1.164 * (Y - 16) + 2.018 * (U - 128); //1.164 * (Y - 16) + 1.596 * (V - 128);
			//rgb[index++] = 1.164 * (Y - 16) - 0.813 * (V - 128) - 0.391 * (U - 128);
			//rgb[index++] = 1.164 * (Y - 16) + 1.596 * (V - 128); //1.164 * (Y - 16) + 2.018 * (U - 128);

			rgb[index++] = (1164 * (Y - 16) + 2018 * (U - 128))/1000>255 ? 255: (1164 * (Y - 16) + 2018 * (U - 128))/1000;
			rgb[index++] = (1164 * (Y - 16) - 813 * (V - 128) - 391 * (U - 128))/1000 > 255 ? 255 : (1164 * (Y - 16) - 813 * (V - 128) - 391 * (U - 128))/1000;
			rgb[index++] = (1164 * (Y - 16) + 1596 * (V - 128))/1000 > 255 ? 255 : (1164 * (Y - 16) + 1596 * (V - 128))/1000;

			
			//rgb[index++] = 1.164 * (Y - 0) + 2.018 * (U - 128);  //1.164 * (Y - Offset_Y) + 1.596 * (V - Offset_V);
			//rgb[index++] = 1.164 * (Y - 0) - 0.813 * (V - 128) - 0.391 * (U - 128);
			//rgb[index++] = 1.164 * (Y - 0) + 1.596 * (V - 128);//1.164 * (Y - Offset_Y) + 2.018 * (U - Offset_U);

        }
    }
}


static int Table_fv1[256] = { -180, -179, -177, -176, -174, -173, -172, -170, -169, -167, -166, -165, -163, -162, -160, -159, -158, -156, -155, -153, -152, -151, -149, -148, -146, -145, -144, -142, -141, -139, -138, -137,  -135, -134, -132, -131, -130, -128, -127, -125, -124, -123, -121, -120, -118, -117, -115, -114, -113, -111, -110, -108, -107, -106, -104, -103, -101, -100, -99, -97, -96, -94, -93, -92, -90,  -89, -87, -86, -85, -83, -82, -80, -79, -78, -76, -75, -73, -72, -71, -69, -68, -66, -65, -64,-62, -61, -59, -58, -57, -55, -54, -52, -51, -50, -48, -47, -45, -44, -43, -41, -40, -38, -37,  -36, -34, -33, -31, -30, -29, -27, -26, -24, -23, -22, -20, -19, -17, -16, -15, -13, -12, -10, -9, -8, -6, -5, -3, -2, 0, 1, 2, 4, 5, 7, 8, 9, 11, 12, 14, 15, 16, 18, 19, 21, 22, 23, 25, 26, 28, 29, 30, 32, 33, 35, 36, 37, 39, 40, 42, 43, 44, 46, 47, 49, 50, 51, 53, 54, 56, 57, 58, 60, 61, 63, 64, 65, 67, 68, 70, 71, 72, 74, 75, 77, 78, 79, 81, 82, 84, 85, 86, 88, 89, 91, 92, 93, 95, 96, 98, 99, 100, 102, 103, 105, 106, 107, 109, 110, 112, 113, 114, 116, 117, 119, 120, 122, 123, 124, 126, 127, 129, 130, 131, 133, 134, 136, 137, 138, 140, 141, 143, 144, 145, 147, 148,  150, 151, 152, 154, 155, 157, 158, 159, 161, 162, 164, 165, 166, 168, 169, 171, 172, 173, 175, 176, 178 };
static int Table_fv2[256] = { -92, -91, -91, -90, -89, -88, -88, -87, -86, -86, -85, -84, -83, -83, -82, -81, -81, -80, -79, -78, -78, -77, -76, -76, -75, -74, -73, -73, -72, -71, -71, -70, -69, -68, -68, -67, -66, -66, -65, -64, -63, -63, -62, -61, -61, -60, -59, -58, -58, -57, -56, -56, -55, -54, -53, -53, -52, -51, -51, -50, -49, -48, -48, -47, -46, -46, -45, -44, -43, -43, -42, -41, -41, -40, -39, -38, -38, -37, -36, -36, -35, -34, -33, -33, -32, -31, -31, -30, -29, -28, -28, -27, -26, -26, -25, -24, -23, -23, -22, -21, -21, -20, -19, -18, -18, -17, -16, -16, -15, -14, -13, -13, -12, -11, -11, -10, -9, -8, -8, -7, -6, -6, -5, -4, -3, -3, -2, -1, 0, 0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 22, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 42, 43, 44, 45, 45, 46, 47, 47, 48, 49, 50, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 62, 63, 64, 65, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 72, 73, 74, 75, 75, 76, 77, 77, 78, 79, 80, 80, 81, 82, 82, 83, 84, 85, 85, 86, 87, 87, 88, 89, 90, 90 };
static int Table_fu1[256] = { -44, -44, -44, -43, -43, -43, -42, -42, -42, -41, -41, -41, -40, -40, -40, -39, -39, -39, -38, -38, -38, -37, -37, -37, -36, -36, -36, -35, -35, -35, -34, -34, -33, -33, -33, -32, -32, -32, -31, -31, -31, -30, -30, -30, -29, -29, -29, -28, -28, -28, -27, -27, -27, -26, -26, -26, -25, -25, -25, -24, -24, -24, -23, -23, -22, -22, -22, -21, -21, -21, -20, -20, -20, -19, -19, -19, -18, -18, -18, -17, -17, -17, -16, -16, -16, -15, -15, -15, -14, -14, -14, -13, -13, -13, -12, -12, -11, -11, -11, -10, -10, -10, -9, -9, -9, -8, -8, -8, -7, -7, -7, -6, -6, -6, -5, -5, -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -1, -1, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43 };
static int Table_fu2[256] = { -227, -226, -224, -222, -220, -219, -217, -215, -213, -212, -210, -208, -206, -204, -203, -201, -199, -197, -196, -194, -192, -190, -188, -187, -185, -183, -181, -180, -178, -176, -174, -173, -171, -169, -167, -165, -164, -162, -160, -158, -157, -155, -153, -151, -149, -148, -146, -144, -142, -141, -139, -137, -135, -134, -132, -130, -128, -126, -125, -123, -121, -119, -118, -116, -114, -112, -110, -109, -107, -105, -103, -102, -100, -98, -96, -94, -93, -91, -89, -87, -86, -84, -82, -80, -79, -77, -75, -73, -71, -70, -68, -66, -64, -63, -61, -59, -57, -55, -54, -52, -50, -48, -47, -45, -43, -41, -40, -38, -36, -34, -32, -31, -29, -27, -25, -24, -22, -20, -18, -16, -15, -13, -11, -9, -8, -6, -4, -2, 0, 1, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 21, 23, 24, 26, 28, 30, 31, 33, 35, 37, 39, 40, 42, 44, 46, 47, 49, 51, 53, 54, 56, 58, 60, 62, 63, 65, 67, 69, 70, 72, 74, 76, 78, 79, 81, 83, 85, 86, 88, 90, 92, 93, 95, 97, 99, 101, 102, 104, 106, 108, 109, 111, 113, 115, 117, 118, 120, 122, 124, 125, 127, 129, 131, 133, 134, 136, 138, 140, 141, 143, 145, 147, 148, 150, 152, 154, 156, 157, 159, 161, 163, 164, 166, 168, 170, 172, 173, 175, 177, 179, 180, 182, 184, 186, 187, 189, 191, 193, 195, 196, 198, 200, 202, 203, 205, 207, 209, 211, 212, 214, 216, 218, 219, 221, 223, 225 };

void ConvertYUV420SPToBGR(unsigned char* src,unsigned char* Dst,int ImageWidth,int ImageHeight)
{
    if (ImageWidth < 1 || ImageHeight < 1 || src == NULL || Dst == NULL)
        return ;
    const long len = ImageWidth * ImageHeight;
    unsigned char* yData = src;
    unsigned char* vData = &yData[len];
    unsigned char* uData = &vData[len >> 2];
    int bgr[3];
    int yIdx,uIdx,vIdx,idx;
    int rdif,invgdif,bdif;
    for (int i = 0;i < ImageHeight;i++){
        for (int j = 0;j < ImageWidth;j++){
            yIdx = i * ImageWidth + j;
            vIdx = (i/2) * (ImageWidth/2) + (j/2);
            uIdx = vIdx;
 
            rdif = Table_fv1[vData[vIdx]];
            invgdif = Table_fu1[uData[uIdx]] + Table_fv2[vData[vIdx]];
            bdif = Table_fu2[uData[uIdx]];
 
            bgr[0] = yData[yIdx] + bdif;    
            bgr[1] = yData[yIdx] - invgdif;
            bgr[2] = yData[yIdx] + rdif;
 
            for (int k = 0;k < 3;k++){
                idx = (i * ImageWidth + j) * 3 + k;
                if(bgr[k] >= 0 && bgr[k] <= 255)
                    Dst[idx] = bgr[k];
                else
                    Dst[idx] = (bgr[k] < 0)?0:255;
            }
        }
    }
}


#if 1
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

static int jzfb_dev_init(struct jzfb_dev * jzfb_dev)
{
	int ret = 0;
	int i, j;

	jzfb_dev->fd = open(FB0DEV, O_RDWR);
	if (jzfb_dev->fd <= 2) {
		perror("fb0 open error");
		return jzfb_dev->fd;
	}

	/* get framebuffer's var_info */
	if ((ret = ioctl(jzfb_dev->fd, FBIOGET_VSCREENINFO, &jzfb_dev->var_info)) < 0) {
		perror("FBIOGET_VSCREENINFO failed");
		goto err_getinfo;
	}

	/* get framebuffer's fix_info */
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
#endif

#if 0 //Lable 文字
void lv_example_get_started_1(void)
{
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}
#endif


#if 1 //按钮事件函数

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

/**
 * Create a button with a label and react on click event.
 */
void lv_example_get_started_2(void)
{
    lv_obj_t * btn = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
    //lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
	lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 0);	//按键位置
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/
    lv_obj_center(label);
}
#endif

#if 0 //按钮风格改变
static lv_style_t style_btn; //
static lv_style_t style_button_pressed; //按下的样式
static lv_style_t style_button_red; //红色样式

static lv_color_t darken(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa)
{
    LV_UNUSED(dsc);
    return lv_color_darken(color, opa);
}

static void style_init(void)
{
    /*Create a simple button style*/
    lv_style_init(&style_btn); //
    lv_style_set_radius(&style_btn, 10); //按钮的半径
    lv_style_set_bg_opa(&style_btn, LV_OPA_100); //按钮上面的背景颜色深度
    lv_style_set_bg_color(&style_btn, lv_palette_lighten(LV_PALETTE_YELLOW, 3));
    lv_style_set_bg_grad_color(&style_btn, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);

    lv_style_set_border_color(&style_btn, lv_color_black());
    lv_style_set_border_opa(&style_btn, LV_OPA_20);
    lv_style_set_border_width(&style_btn, 2);

    lv_style_set_text_color(&style_btn, lv_color_black());

    /*Create a style for the pressed state.
     *Use a color filter to simply modify all colors in this state*/
    static lv_color_filter_dsc_t color_filter;
    lv_color_filter_dsc_init(&color_filter, darken);
    lv_style_init(&style_button_pressed);
    lv_style_set_color_filter_dsc(&style_button_pressed, &color_filter);
    lv_style_set_color_filter_opa(&style_button_pressed, LV_OPA_20);

    /*Create a red style. Change only some colors.*/
    lv_style_init(&style_button_red);
    lv_style_set_bg_color(&style_button_red, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_button_red, lv_palette_lighten(LV_PALETTE_RED, 3));
}

/**
 * Create styles from scratch for buttons.
 */
void lv_example_get_started_3(void)
{
    /*Initialize the style*/
    style_init();

    /*Create a button and use the new styles*/
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    /* Remove the styles coming from the theme
     * Note that size and position are also stored as style properties
     * so lv_obj_remove_style_all will remove the set size and position too */
    lv_obj_remove_style_all(btn);
    lv_obj_set_pos(btn, 10, 10);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_add_style(btn, &style_button_pressed, LV_STATE_PRESSED);

    /*Add a label to the button*/
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);

    /*Create another button and use the red style too*/
    lv_obj_t * btn2 = lv_button_create(lv_screen_active());
    lv_obj_remove_style_all(btn2);                      /*Remove the styles coming from the theme*/
    lv_obj_set_pos(btn2, 10, 80);
    lv_obj_set_size(btn2, 120, 50);
    lv_obj_add_style(btn2, &style_btn, 0);
    lv_obj_add_style(btn2, &style_button_red, 0);
    lv_obj_add_style(btn2, &style_button_pressed, LV_STATE_PRESSED);
    lv_obj_set_style_radius(btn2, LV_RADIUS_CIRCLE, 0); /*Add a local style too*/

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Button 2");
    lv_obj_center(label);
}
#endif


#if 0

#define CANVAS_WIDTH  200
#define CANVAS_HEIGHT  150

void lv_example_canvas_1(void)
{
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.radius = 15;
    rect_dsc.bg_opa = LV_OPA_COVER; //设置背景的透明度
    rect_dsc.bg_grad.dir = LV_GRAD_DIR_VER; //
    rect_dsc.bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_RED);
    rect_dsc.bg_grad.stops[0].opa = LV_OPA_100;
    rect_dsc.bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_BLUE);
    rect_dsc.bg_grad.stops[1].opa = LV_OPA_50;
    rect_dsc.border_width = 2;
    rect_dsc.border_opa = LV_OPA_90;
    rect_dsc.border_color = lv_color_white();
    rect_dsc.shadow_width = 5;
    rect_dsc.shadow_offset_x = 5;
    rect_dsc.shadow_offset_y = 5;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_palette_main(LV_PALETTE_ORANGE);
    label_dsc.text = "Some text on text canvas";


	
    /*Create a buffer for the canvas*/
    LV_DRAW_BUF_DEFINE(draw_buf_16bpp, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_RGB565);

    lv_obj_t * canvas = lv_canvas_create(lv_screen_active()); //创建一个画布
    lv_canvas_set_draw_buf(canvas, &draw_buf_16bpp); //设置画布的缓存
    lv_obj_center(canvas); //画布居中放置
    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER); //画布背景填充颜色

    lv_layer_t layer; //创建一个图层
    lv_canvas_init_layer(canvas, &layer); //图层初始化到画布上面

#if 1
    lv_area_t coords_rect = {30, 20, 100, 70}; //创建矩形的位置
    lv_draw_rect(&layer, &rect_dsc, &coords_rect); //绘制一个矩形

    lv_area_t coords_text = {10, 90, 100, 100}; //左边距，上边距，
    lv_draw_label(&layer, &label_dsc, &coords_text);

    lv_canvas_finish_layer(canvas, &layer);
#endif

#if 1
    /*Test the rotation. It requires another buffer where the original image is stored.
     *So use previous canvas as image and rotate it to the new canvas*/
    LV_DRAW_BUF_DEFINE(draw_buf_32bpp, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_ARGB8888);
    /*Create a canvas and initialize its palette*/
    canvas = lv_canvas_create(lv_screen_active()); //创建一个画布canvas
    lv_canvas_set_draw_buf(canvas, &draw_buf_32bpp); //设置画布的缓冲区，用户保存图像
    lv_canvas_fill_bg(canvas, lv_color_hex3(0xccc), LV_OPA_COVER); //画布背景填充的颜色
    lv_obj_center(canvas); //画布居中排列

    lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_OPA_COVER); //画布背景渐变排线

    lv_canvas_init_layer(canvas, &layer); //
    lv_image_dsc_t img;
    lv_draw_buf_to_image(&draw_buf_16bpp, &img);
    lv_draw_image_dsc_t img_dsc;
    lv_draw_image_dsc_init(&img_dsc);
    img_dsc.rotation = 1800; //图像旋转角度
    img_dsc.src = &img; //图像的源
    img_dsc.pivot.x = CANVAS_WIDTH / 2; //图像旋转的中心点x轴
    img_dsc.pivot.y = CANVAS_HEIGHT / 2; 

    lv_area_t coords_img = {0, 0, CANVAS_WIDTH - 1, CANVAS_HEIGHT - 1};


    //lv_draw_image(&layer, &img_dsc, &coords_img);
	lv_draw_image(&layer, &img_dsc, &coords_img);


    lv_canvas_finish_layer(canvas, &layer);
#endif


}

#endif

#define CANVAS_WIDTH  320
#define CANVAS_HEIGHT  480

#define CANVAS_WIDTH_IMG 100
#define CANVAS_HEIGHT_IMG  100

#if 0
/**
 * Draw an image to the canvas
 */
void lv_example_canvas_6(void)
{

    /*Create a buffer for the canvas*/
    static uint8_t cbuf[LV_CANVAS_BUF_SIZE(CANVAS_WIDTH, CANVAS_HEIGHT, 32, LV_DRAW_BUF_STRIDE_ALIGN)];

    /*Create a canvas and initialize its palette*/
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active()); //创建画布
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_ARGB8888); //设置画布缓存
    lv_canvas_fill_bg(canvas, lv_color_hex3(0xccc), LV_OPA_COVER); //画布背景色
    lv_obj_center(canvas); //画布居中显示

    lv_layer_t layer; //设置图层
    lv_canvas_init_layer(canvas, &layer); //图层绑定画布

    LV_IMAGE_DECLARE(img_lvgl_logo); //声明图片
    lv_draw_image_dsc_t dsc; //创建动态图片
    lv_draw_image_dsc_init(&dsc); //初始化动态图片
    dsc.src = &img_lvgl_logo; //给图片

    lv_area_t coords = {10, 10, 10 + img_lvgl_logo.header.w - 1, 10 + img_lvgl_logo.header.h - 1};

    lv_draw_image(&layer, &dsc, &coords);

	lv_canvas_finish_layer(canvas, &layer);

	sleep(2);

	LV_IMAGE_DECLARE(img_demo_widgets_avatar); //声明图片
    dsc.src = &img_demo_widgets_avatar; //给图片

    lv_area_t coords1 = {10, 10, 10 + img_demo_widgets_avatar.header.w - 1, 10 + img_demo_widgets_avatar.header.h - 1};

    lv_draw_image(&layer, &dsc, &coords1);

	lv_canvas_finish_layer(canvas, &layer);

}
#endif


/*
void my_input_read(lv_indev_t * indev, lv_indev_data_t*data)
{
  if(touchpad_pressed) {
    data->point.x = touchpad_x;
    data->point.y = touchpad_y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}
*/

int lvgl_main(void)
{
    //printf("--------test--------\n");
    lv_init();

	//触摸操作
	/*Register at least one display before you register any input devices*/
	lv_indev_t * indev = lv_indev_create(); //
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);   /*See below.*/
	//lv_indev_set_read_cb(indev, read_cb);  /*See below.*/

    /*Linux frame buffer device init*/
    lv_display_t * disp = lv_linux_fbdev_create(); //分辨率
    lv_linux_fbdev_set_file(disp, "/dev/fb0");

    /*Create a Demo*/
    //lv_demo_widgets();
    //lv_demo_widgets_start_slideshow();

    /*Handle LVGL tasks*/

	//lv_example_get_started_1();
	lv_example_get_started_2();
	//lv_example_get_started_3();
	//lv_example_canvas_1();
	//lv_example_canvas_6();

#if 0 //test OK
    /*Create a buffer for the canvas*/
    static uint8_t cbuf[LV_CANVAS_BUF_SIZE(CANVAS_WIDTH, CANVAS_HEIGHT, 32, LV_DRAW_BUF_STRIDE_ALIGN)];

    /*Create a canvas and initialize its palette*/
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active()); //创建画布
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_RGB888); //设置画布缓存
    lv_canvas_fill_bg(canvas, lv_color_hex3(0xccc), LV_OPA_COVER); //画布背景色
    lv_obj_center(canvas); //画布居中显示

    lv_layer_t layer; //设置图层
    lv_canvas_init_layer(canvas, &layer); //图层绑定画布
    LV_IMAGE_DECLARE(img_lvgl_test); //声明图片-下层图片，摄像头内容
    lv_draw_image_dsc_t dsc; //创建动态图片
    lv_draw_image_dsc_init(&dsc); //初始化动态图片
	//dsc.rotation = 1800; //图像旋转角度
    dsc.src = &img_lvgl_test; //给图片
	lv_area_t coords = {0, 0, img_lvgl_test.header.w, img_lvgl_test.header.h};
	lv_draw_image(&layer, &dsc, &coords);
	lv_canvas_finish_layer(canvas, &layer);

	//添加一张图片
	LV_IMAGE_DECLARE(iris_unlock_main);  //上层图片
	lv_obj_t * ir_img = lv_image_create(lv_screen_active());
	lv_image_set_rotation(ir_img, 1800); //图片旋转180度
	lv_image_set_src(ir_img, &iris_unlock_main);
#endif

    while(1) {
        lv_timer_handler();
        usleep(500);
		
		#if 0
		if (1 == g_swap_img_buffer_flag)
		{
			lv_draw_image(&layer, &dsc, &coords);
			lv_canvas_finish_layer(canvas, &layer);
			g_swap_img_buffer_flag = 0;
			//printf("g_swap_img_buffer_flag:%d\n", g_swap_img_buffer_flag);
		}
		#endif
    }

    return 0;
}


#if 1
int imp_main(void)
{
	int ret,i;
	struct jzfb_dev *jzfb_dev;
	int fram_num = 0;

	IMPFrameInfo *frame_bak;
	IMPFSChnAttr fs_chn_attr[2];

#if 1
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
#endif

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
#if 0
	unsigned char sat_tmp = 0;
    ret = IMP_ISP_Tuning_GetSaturation(0, &sat_tmp);
	sat_tmp = 0;
    ret = IMP_ISP_Tuning_SetSaturation(0, &sat_tmp);
#endif

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


	/* Step.4 set framesource attr config */
	ret = IMP_FrameSource_GetChnAttr(0, &fs_chn_attr[0]);
	if(ret < 0){
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_GetChnAttr failed\n", __func__, __LINE__);
		return -1;
	}

#if 0
	fs_chn_attr[0].pixFmt = PIX_FMT_NV12;
	fs_chn_attr[0].i2dattr.i2d_enable = 1;
	fs_chn_attr[0].i2dattr.rotate_enable = 0;
	fs_chn_attr[0].scaler.enable = 1;
	if (fs_chn_attr[0].i2dattr.rotate_enable == 1){ //旋转
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
#else
	fs_chn_attr[0].pixFmt = PIX_FMT_NV12;
	fs_chn_attr[0].i2dattr.i2d_enable = 1;
	fs_chn_attr[0].i2dattr.rotate_enable = 1; //旋转
	fs_chn_attr[0].scaler.enable = 1; //缩放
	fs_chn_attr[0].i2dattr.mirr_enable = 0; //镜像

	if (fs_chn_attr[0].i2dattr.rotate_enable == 1){ //旋转
		fs_chn_attr[0].scaler.outwidth = jzfb_dev->height; //
		fs_chn_attr[0].scaler.outheight = jzfb_dev->width;
		fs_chn_attr[0].picWidth = jzfb_dev->height;
		fs_chn_attr[0].picHeight = jzfb_dev->width;
		fs_chn_attr[0].i2dattr.rotate_angle = 270;//90;
		}
	else{
		fs_chn_attr[0].scaler.outwidth = jzfb_dev->width; //320
		fs_chn_attr[0].scaler.outheight = jzfb_dev->height; //480
		fs_chn_attr[0].picWidth = jzfb_dev->width;
		fs_chn_attr[0].picHeight = jzfb_dev->height;
		}
	
	printf("picWidth : %d\n", fs_chn_attr[0].picWidth);
	printf("picHeight : %d\n", fs_chn_attr[0].picHeight);
	fs_chn_attr[0].fcrop.enable = 1;
	fs_chn_attr[0].fcrop.left = 240;//160;
	fs_chn_attr[0].fcrop.top = 0;
	fs_chn_attr[0].fcrop.width = 1440; //1620;
	fs_chn_attr[0].fcrop.height = 1080;

#endif

	ret = IMP_FrameSource_SetChnAttr(0, &fs_chn_attr[0]);
	if(ret < 0) {
		IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_SetChnAttr failed\n", __func__, __LINE__);
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
	#if 0
	char file_name[] = "rgb.bmp";
	char file_name1[] = "yuv";
	int fd = -1;
	int fd1 = -1;
	int res = 0;
	//fd = open(file_name, O_RDWR | O_CREAT, 0777);
	//fd1 = open(file_name1, O_RDWR | O_CREAT, 0777);
	//printf("---------------fd:%d-----------\n", fd);
	#endif
	printf("---------------display-----------\n");
	/* Step.6 Display  */
	while(1) {

		#if 1

		if (0==g_swap_img_buffer_flag)
		{
			ret = IMP_FrameSource_GetFrame(0, &frame_bak);
			if(ret < 0){
				IMP_LOG_ERR(TAG, "%s(%d):IMP_FrameSource_GetFrame failed\n", __func__, __LINE__);
				return -1;
			}
			//ConvertYUV420SPToBGR((void *)(frame_bak->virAddr), &img_lvgl_rgb888_map[0], 320, 480);
			NV12_TO_RGB24((void *)(frame_bak->virAddr), &img_lvgl_rgb888_map[0], 320, 480);
			//memcpy(&img_lvgl_rgb888_map[0], (void *)(frame_bak->virAddr), frame_bak->size);
			IMP_FrameSource_ReleaseFrame(0, frame_bak);

			#if 0
			if (fd != 0)
			{
				printf("-----write file start---\n");
				res = write(fd, img_lvgl_rgb888_map, sizeof(img_lvgl_rgb888_map));
				{
					printf("%s file open fail,errno = %d.\r\n", file_name, errno);
        			return -1;
				}					
					
				printf("-----write file end------\n");
			}
			close(fd);
			fd = 0;
			#endif

			g_swap_img_buffer_flag = 1;
			
			//printf("frame_bak->size:%u \n", frame_bak->size);
		}
		else
		{
			usleep(500);
		}
		#else

		switch(fram_num) {
		case 0:
			//printf("-------------\n");
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

		if (0==g_swap_img_buffer_flag)
		{
			memcpy(g_lvgl_swap_img.p_img_data, (void *)(frame_bak->virAddr), frame_bak->size);
			IMP_FrameSource_ReleaseFrame(0, frame_bak);
			g_swap_img_buffer_flag = 1;
			printf("g_swap_img_buffer_flag:%d\n", g_swap_img_buffer_flag);
		}

		if (ret = jzfb_pan_display(jzfb_dev, fram_num))
			break;
		fram_num ++;
		if (fram_num > 1)
			fram_num = 0;

		IMP_FrameSource_ReleaseFrame(0, frame_bak);
		#endif
	}

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
#endif

int main(void)
{
	int ret = 0;

	pthread_t p_LVGL;
	pthread_t p_IMP;



	printf("------------------------------------------------------\n");
	//摄像头获取数据
	//ret = pthread_create(&p_IMP, NULL, imp_main, NULL);
	//sleep(2);
	//LVGL显示数据
	ret = pthread_create(&p_LVGL, NULL, lvgl_main, NULL);

    while (1)
	{
		//printf("g_swap_img_buffer_flag:%d\n", g_swap_img_buffer_flag);
		sleep(1);
	}
	pthread_exit(p_IMP);
	pthread_exit(p_LVGL);

    return 0;
}



