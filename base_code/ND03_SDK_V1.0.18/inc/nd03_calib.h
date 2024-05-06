/**
 * @file nd03_calib.h
 * @author tongsheng.tang
 * @brief ND03 Calibration functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */

#ifndef __ND03_CALIB_H__
#define __ND03_CALIB_H__

#include "nd03_def.h"

/** @defgroup ND03_Calibration_Group ND03 Calibration Funtions
 *  @brief ND03 Calibration Funtions
 *  @{
 */

/** 获取错误值 */
int32_t ND03_ReadErrorStatus(ND03_Dev_t *pNxDevice);

/** 发送Offset标定请求 */
int32_t ND03_OffsetCalibrationRequest(ND03_Dev_t *pNxDevice);

/** 发送串扰标定请求 */
int32_t ND03_XTalkCalibrationRequest(ND03_Dev_t *pNxDevice);

/** 等待Offset标定完成 */
int32_t ND03_WaitforOffsetCalibration(ND03_Dev_t *pNxDevice);

/** 等待串扰标定完成 */
int32_t ND03_WaitforXTalkCalibration(ND03_Dev_t *pNxDevice);

/** Offset标定函数(弃用)*/
int32_t ND03_Calibration(ND03_Dev_t *pNxDevice);

/** Offset标定标定函数*/
int32_t ND03_OffsetCalibration(ND03_Dev_t *pNxDevice);

/** Offset标定函数带有标定深度设置(弃用) */
int32_t ND03_CalibrationAtDepth(ND03_Dev_t *pNxDevice, uint16_t calib_depth_mm);

/** Offset标定函数带有标定深度设置 */
int32_t ND03_OffsetCalibrationAtDepth(ND03_Dev_t *pNxDevice, uint16_t calib_depth_mm);

/** 串扰标定函数 */
int32_t ND03_XTalkCalibration(ND03_Dev_t *pNxDevice);

/** 串扰标定函数带有标定深度设置 */
int32_t ND03_XTalkCalibrationAtDepth(ND03_Dev_t *pNxDevice, uint16_t xtalk_calib_depth_mm);

/** 清除串扰值 */
int32_t ND03_ClearXTalkAmp(ND03_Dev_t *pNxDevice);

/** 设置串扰值 */
int32_t ND03_SetXTalkAmp(ND03_Dev_t *pNxDevice, int32_t xtalk_amp);

/** 获取串扰值 */
int32_t ND03_GetXTalkAmp(ND03_Dev_t *pNxDevice, int32_t *xtalk_amp);

/** 设置Offset标定深度 */
int32_t ND03_SetOffsetCalibDistMM(ND03_Dev_t *pNxDevice, uint16_t depth_mm);

/** 设置串扰标定深度 */
int32_t ND03_SetXTalkCalibDistMM(ND03_Dev_t *pNxDevice, uint16_t depth_mm);

/** @} ND03_Calibration_Group */

#endif

