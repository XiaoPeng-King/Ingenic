/**
 * @file nd03_data.h
 * @author tongsheng.tang
 * @brief ND03 get depth data functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */
#ifndef __ND03_DATA_H__
#define __ND03_DATA_H__

#include "nd03_def.h"
 
/** @defgroup ND03_Data_Group ND03 Data Funtions
 *  @brief ND03 Data Funtions
 *  @{
 */

/** 等待ND03测距完成 */
int32_t ND03_WaitDepthAndAmpBufReady(ND03_Dev_t *pNxDevice);
/** 检测ND03测距是否完成 */
int32_t ND03_GetDepthAndAmpBufReady(ND03_Dev_t *pNxDevice);
/** 发送开始测量信号 */
int32_t ND03_StartMeasurement(ND03_Dev_t *pNxDevice);
/** 发送结束测量信号，用于连续模式 */
int32_t ND03_StopMeasurement(ND03_Dev_t *pNxDevice);
/** 读取深度和幅度值 */
int32_t ND03_ReadDepthAndAmpData(ND03_Dev_t *pNxDevice, ND03_RangingData_t *pData);
/** 清除数据有效位 */
int32_t ND03_ClearDataValidFlag(ND03_Dev_t *pNxDevice);
/** 获取一次测量深度和幅度值 */
int32_t ND03_GetRangingData(ND03_Dev_t *pNxDevice, ND03_RangingData_t *pData);

/** @} ND03_Data_Group */

#endif

