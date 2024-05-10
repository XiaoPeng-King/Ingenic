/**
 * @file nd03_dev.h
 * @author tongsheng.tang
 * @brief ND03 device setting functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */
#ifndef __ND03_DEV_H__
#define __ND03_DEV_H__

#include "nd03_def.h"

/** @defgroup ND03_Dev_Group ND03 Device Funtions
 *  @brief ND03 Device Funtions
 *  @{
 */

/** 获取SDK版本号 */
uint32_t ND03_GetSdkVersion(void);
/** 设置测量模式 */
int32_t ND03_SetRangeMode(ND03_Dev_t *pNxDevice, uint32_t mode);
/** 获取当前测量模式 */
uint32_t ND03_GetRangeMode(ND03_Dev_t *pNxDevice);
/** 设置ND03的设备地址 */
int32_t ND03_SetSlaveAddr(ND03_Dev_t *pNxDevice, uint8_t addr);
/** 获取ND03的设备地址 */
int32_t ND03_GetSlaveAddr(ND03_Dev_t *pNxDevice, uint8_t *addr);
/** 设置ND03调制频率 */
int32_t ND03_SetModFreq(ND03_Dev_t *pNxDevice, uint32_t freq);
/** 获取ND03调制频率 */
int32_t ND03_GetModFreq(ND03_Dev_t *pNxDevice, uint32_t *freq);
/** 设置非线性补偿状态 */
int32_t ND03_SetOffsetState(ND03_Dev_t *pNxDevice, ND03_Status_e flag);
/** 获取非线性补偿状态 */
int32_t ND03_GetOffsetState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag);
/** 设置自动曝光状态 */
int32_t ND03_SetAutoExpState(ND03_Dev_t *pNxDevice, ND03_Status_e flag);
/** 获取自动曝光状态 */
int32_t ND03_GetAutoExpState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag);
/** 设置串扰矫正状态 */
int32_t ND03_SetXTalkState(ND03_Dev_t *pNxDevice, ND03_Status_e flag);
/** 获取串扰矫正状态 */
int32_t ND03_GetXTalkState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag);
/** 设置温度补偿状态 */
int32_t ND03_SetThermCompensationState(ND03_Dev_t *pNxDevice, ND03_Status_e flag);
/** 获取温度补偿状态 */
int32_t ND03_GetThermCompensationState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag);
/** 配置GPIO的功能 */
int32_t ND03_SetGpioConfig(ND03_Dev_t *pNxDevice, ND03_GpioFunc_t functionality, uint32_t polarity);
/** 获取GPIO的功能配置 */
int32_t ND03_GetGpioConfig(ND03_Dev_t *pNxDevice, ND03_GpioFunc_t *functionality, uint32_t *polarity);
/** 设置最大积分时间 */
int32_t ND03_SetMaxExposure(ND03_Dev_t *pNxDevice, uint16_t max_exposure_us);
/** 获取最大积分时间的值 */
int32_t ND03_GetMaxExposure(ND03_Dev_t *pNxDevice, uint16_t *max_exposure_us);
/** 设置当前ND03的积分时间 */
int32_t ND03_SetCurrentExp(ND03_Dev_t *pNxDevice, uint16_t current_exp);
/** 获取当前ND03的积分时间 */
uint16_t ND03_GetCurrentExp(ND03_Dev_t *pNxDevice);
/** 获取当前ND03的温度 */
int16_t ND03_GetCurrentTherm(ND03_Dev_t *pNxDevice);
/** 硬件待机(弃用) */
void ND03_HardwareSleep(ND03_Dev_t *pNxDevice);
/** 硬件待机唤醒(弃用) */
void ND03_HardwareWakeup(ND03_Dev_t *pNxDevice);
/** 软件待机(弃用) */
int32_t ND03_SoftwareSleep(ND03_Dev_t *pNxDevice);
/** 软件待机唤醒(弃用) */
int32_t ND03_SoftwareWakeup(ND03_Dev_t *pNxDevice);
/** ND03的flash锁 */
int32_t ND03_FlashLock(ND03_Dev_t *pNxDevice, int32_t enable);
/** ND03的flash进入低功耗 */
int32_t ND03_FlashPowerDown(ND03_Dev_t *pNxDevice);
/** ND03的flash从低功耗中唤醒 */
int32_t ND03_FlashPowerWakeup(ND03_Dev_t *pNxDevice);
/** ND03待机,优化使用XShut待机 */
int32_t ND03_Sleep(ND03_Dev_t *pNxDevice);
/** ND03唤醒 */
int32_t ND03_Wakeup(ND03_Dev_t *pNxDevice);
/** ND03深度睡眠 */
int32_t ND03_DeepPowerSleep(ND03_Dev_t *pNxDevice);
/** ND03深度睡眠唤醒 */
int32_t ND03_DeepPowerWakeup(ND03_Dev_t *pNxDevice);
/** ND03设置普通温度系数 */
int32_t ND03_SetNormalThermCoe(ND03_Dev_t *pNxDevice, int32_t therm_coe_normal);
/** ND03设置快速温度系数 */
int32_t ND03_SetFastThermCoe(ND03_Dev_t *pNxDevice, int32_t therm_coe_fast);
/** 设置目标幅度值 */
int32_t ND03_SetTargetAmp(ND03_Dev_t *pNxDevice, int32_t target_amp);
/** 设置目标幅度容错值 */
int32_t ND03_SetTargetAmpError(ND03_Dev_t *pNxDevice, int32_t target_amp_error);
/** 初始化ND03设备 */
int32_t ND03_InitDevice(ND03_Dev_t *pNxDevice);
/** 获取ND03模组固件版本号 */
int32_t ND03_GetFirmwareVersion(ND03_Dev_t *pNxDevice, uint32_t* pFirmwareDataBuf);
/** 获取设备当前信息 */
int32_t ND03_GetDevInfo(ND03_Dev_t *pNxDevice);
/** 获取ND03模组中flash数据 */
int32_t ND03_GetFlashData(ND03_Dev_t *pNxDevice, uint32_t flash_addr, uint32_t* flash_rdata);
/** 等待ND03模组启动 */
int32_t ND03_WaitDeviceBootUp(ND03_Dev_t *pNxDevice);
/** ND03发生异常错误后重启 */
int32_t ND03_RebootOnAbnormalError(ND03_Dev_t *pNxDevice);

/** @} ND03_Dev_Group */

#endif

