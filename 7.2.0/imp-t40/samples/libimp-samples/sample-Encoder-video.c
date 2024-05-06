/*
 * sample-Encoder-video.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 *
 * The specific description of all APIs called in this file can be viewed in the header file in the proj/sdk-lv3/include/api/cn/imp/ directory
 *
 * Step.1 System init System initialization
 *		@code
 *			memset(&sensor_info, 0, sizeof(sensor_info));
 *			if(SENSOR_NUM == IMPISP_TOTAL_ONE){
 *				memcpy(&sensor_info[0], &Def_Sensor_Info[0], sizeof(IMPSensorInfo));
 *			} else if(SENSOR_NUM == IMPISP_TOTAL_TWO){
 *				memcpy(&sensor_info[0], &Def_Sensor_Info[0], sizeof(IMPSensorInfo) * 2);
 *			}else if(SENSOR_NUM ==IMPISP_TOTAL_THR){
 *				memcpy(&sensor_info[0], &Def_Sensor_Info[0], sizeof(IMPSensorInfo) * 3)
 *			} //According to the number of sensors, copy the contents of the corresponding size Def_Sensor_Info to sensor_info
 *
 *			ret = IMP_ISP_Open() //Open ISP module
 *			ret = IMP_ISP_SetCameraInputMode(&mode) //If there are multiple sensors (maximum support of three cameras), set the mode to multiple cameras (please ignore if you're single camera)
 *			ret = IMP_ISP_AddSensor(IMPVI_MAIN, &sensor_info[*]) //Add sensor, before this operation the sensor driver has been added to the kernel (IMPVI_MAIN is the main camera, IMPVI_SEC is the second camera, IMPVI_THR is the third camera)
 *			ret = IMP_ISP_EnableSensor(IMPVI_MAIN, &sensor_info[*])	//enable sensor, now sensor starts to output image (IMPVI_MAIN is the main camera, IMPVI_SEC is the second camera, IMPVI_THR is the third camera.)
 *			ret = IMP_System_Init() //System initialization
 *			ret = IMP_ISP_EnableTuning() //enable ISP tuning, the ISP debugging interface can then be called
 *		@endcode
 * Step.2 FrameSource init Framesource initialization
 *		@code
 *			ret = IMP_FrameSource_CreateChn(chn[i].index, &chn[i].fs_chn_attr) //Create channel
 *			ret = IMP_FrameSource_SetChnAttr(chn[i].index, &chn[i].fs_chn_attr) //Set channel-related properties, including: image width, image height, image format, output frame rate of the channel, number of cache buf, cropping and scaling properties
 *		@endcode
 * Step.3 Encoder init Encoding initialization
 *		@code
 *			ret = IMP_Encoder_CreateGroup(chn[i].index) //Create coding group
 *			ret = IMP_Encoder_SetDefaultParam(&channel_attr, chn[i].payloadType, S_RC_METHOD, imp_chn_attr_tmp->picWidth, imp_chn_attr_tmp->picHeight, imp_chn_attr_tmp->outFrmRateNum,
 *											imp_chn_attr_tmp->outFrmRateDen, imp_chn_attr_tmp->outFrmRateNum * 2 / imp_chn_attr_tmp->outFrmRateDen, 2,
 *											(S_RC_METHOD == IMP_ENC_RC_MODE_FIXQP) ? 35 : -1, uTargetBitRate) //Set the encoding chnnel default property
 *			ret = IMP_Encoder_CreateChn(chnNum, &channel_attr) //Create encoding chnnel
 *			ret = IMP_Encoder_RegisterChn(chn[i].index, chnNum) //Register the code Channel to the Group
 *		@endcode
 * Step.4 Bind Bind a framesource and encode channel
 *		@code
 *			ret = IMP_System_Bind(&chn[i].framesource_chn, &chn[i].imp_encoder)	//Bind the framesource and the encoded chnnel, and the data generated by the framesource can be automatically transmitted to the encoded chnnel after the binding is successful
 *		@endcode
 * Step.5 Stream On Enable Framesource chnnel， start outputting the image
 *		@code
 *			ret = IMP_FrameSource_EnableChn(chn[i].index) //Enable channel, channel starts outputting the image
 *		@endcode
 * Step.6 Get stream Obtain the bitstream
 *		@code
 *			ret = IMP_Encoder_StartRecvPic(chnNum) //Encoding channel began to accept images
 *			ret = IMP_Encoder_PollingStream(chnNum, 1000) //Polling the code stream in the encoding channel
 *			ret = IMP_Encoder_GetStream(chnNum, &stream, 1) //Gets the encoding bitstream
 *			ret = save_stream(stream_fd, &stream) //Save the result to an open file descriptor
 *			ret = IMP_Encoder_ReleaseStream(chnNum, &stream) //Release the encoding stream, use it in pairs with the IMP_Encoder_GetStream
 *			ret = IMP_Encoder_StopRecvPic(chnNum) //Encoding channel stop receiving image
 *		@endcode
 * Step.7 Stream Off     disable Framesource chnnel, stop outputting image
 *		@code
 *			ret = IMP_FrameSource_DisableChn(chn[i].index) //disable channel, Channel stops outputting images
 *		@endcode
 * Step.8 UnBind Unbind Framesource and code chnnel
 *		@code
 *			ret = IMP_System_UnBind(&chn[i].framesource_chn, &chn[i].imp_encoder) //Unbind the framesource and encode chnnel
 *		@endcode
 * Step.9 Encoder exit    Encode deinitialization
 *		@code
 *			ret = IMP_Encoder_Query(chnNum, &chn_stat) //Query the encoding channel status
 *			ret = IMP_Encoder_UnRegisterChn(chnNum) //If already registered, unregister the code channel to Group
 *			ret = IMP_Encoder_DestroyChn(chnNum) //destroy encoding chnnel
 *		@endcode
 * Step.10 FrameSource exit Framesource deinitialization
 *		@code
 *			ret = IMP_FrameSource_DestroyChn(chn[i].index) //destory channel
 *		@endcode
 * Step.11 System exit System deinitialization
 *		@code
 *			ret = IMP_ISP_DisableTuning() //disable ISP tuning
 *			ret = IMP_System_Exit() //System deinitialization
 *			ret = IMP_ISP_DisableSensor(IMPVI_MAIN, &sensor_info[*]) //Disable sensor, sensor stops the output image (IMPVI_MAIN is main camera, IMPVI_SEC is second camera, IMPVI_THR is third camera)
 *			ret = IMP_ISP_DelSensor(IMPVI_MAIN, &sensor_info[*]) //Delete sensor (IMPVI_MAIN is main camera, IMPVI_SEC is second camera, IMPVI_THR is third camera)
 *			ret = IMP_ISP_Close() //Close ISP module
 *		@endcode
 * */
#include <stdio.h>
#include <stdlib.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

#include "sample-common.h"

#define TAG "Sample-Encoder-video"

extern struct chn_conf chn[];
static int byGetFd = 0;

int main(int argc, char *argv[])
{
	int i, ret;

    if (argc >= 2) {
        byGetFd = atoi(argv[1]);
    }

	/* Step.1 System init */
	ret = sample_system_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
		return -1;
	}
	IMP_LOG_DBG(TAG, "sample_system_init success\n");

	/* Step.2 FrameSource init */
	ret = sample_framesource_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource init failed\n");
		return -1;
	}

	IMP_LOG_DBG(TAG, "sample_framesource_init success\n");

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

	IMP_LOG_DBG(TAG, "IMP_Encoder_CreateGroup success\n");

	ret = sample_encoder_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder init failed\n");
		return -1;
	}

	IMP_LOG_DBG(TAG, "sample_encoder_init success\n");

	/* Step.4 Bind */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_System_Bind(&chn[i].framesource_chn, &chn[i].imp_encoder);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",i);
				return -1;
			}
		}
	}

	IMP_LOG_DBG(TAG, "IMP_System_Bind success\n");

	/* Step.5 Stream On */
	ret = sample_framesource_streamon();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
		return -1;
	}

	IMP_LOG_DBG(TAG, "sample_framesource_streamon success\n");

	/* Step.6 Get stream */
    if (byGetFd) {
        ret = sample_get_video_stream_byfd();
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "Get video stream byfd failed\n");
            return -1;
        }
	IMP_LOG_DBG(TAG, "sample_get_video_stream_byfd success\n");
    } else {
        ret = sample_get_video_stream();
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "Get video stream failed\n");
            return -1;
        }
	IMP_LOG_DBG(TAG, "sample_get_video_stream success\n");
    }


	/* Exit sequence as follow */
	/* Step.a Stream Off */
	ret = sample_framesource_streamoff();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
		return -1;
	}

	/* Step.b UnBind */
	for (i = 0; i < FS_CHN_NUM; i++) {
		if (chn[i].enable) {
			ret = IMP_System_UnBind(&chn[i].framesource_chn, &chn[i].imp_encoder);
			if (ret < 0) {
				IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",i);
				return -1;
			}
		}
	}

	/* Step.c Encoder exit */
	ret = sample_encoder_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder exit failed\n");
		return -1;
	}

	/* Step.d FrameSource exit */
	ret = sample_framesource_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit failed\n");
		return -1;
	}

	/* Step.e System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}
