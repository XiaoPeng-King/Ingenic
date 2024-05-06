/**
 * @file nd03_dev.c
 * @author tongsheng.tang
 * @brief ND03 device setting functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */

#include "nd03_comm.h"
#include "nd03_dev.h"
#include "nd03_data.h"
#include "nd03_calib.h"
#include "nd03_def.h"



/** SDK主版本 */
static uint8_t sdk_version_major = 1;
/** SDK次版本 */
static uint8_t sdk_version_minor = 0;
/** SDK小版本 */
static uint8_t sdk_version_patch = 18;



/**
 * @brief ND03 Get SDK Version
 *        获取当前SDK的软件版本号
 * @return  uint32_t   
 * @retval  软件版本号
 */
uint32_t ND03_GetSdkVersion(void)
{
    return (uint32_t)sdk_version_major * 10000 + (uint32_t)sdk_version_minor * 100 + (uint32_t)sdk_version_patch;
}

/** 
 * @brief ND03 Set Range Mode
 *        设置测量模式
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param mode: 模式数据，可选参数为：
 *              ND03_RANGE_MODE_NORMAL:普通模式
 *              ND03_RANGE_MODE_FAST:快速模式
 * 
 *              ND03_DEVICEMODE_SINGLE_RANGING:单次测量
 *              ND03_DEVICEMODE_CONTINUOUS_RANGING:连续测量
 * 
 *              ND03_CALCU_MODE_SPOT:单点计算模式
 *              ND03_CALCU_MODE_AVER:平均计算模式
 * @return int32_t
*/
int32_t ND03_SetRangeMode(ND03_Dev_t *pNxDevice, uint32_t mode)
{
    int32_t     ret = ND03_ERROR_NONE;

    /** Set mode reg */
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_RANGE_MODE, mode);

    if(ret == ND03_ERROR_NONE)
    {
        pNxDevice->config.range_mode = mode;

        if((mode & ND03_DEVICEMODE_MASK) == ND03_DEVICEMODE_CONTINUOUS_RANGING)
            ND03_StartMeasurement(pNxDevice);
    }

    return ret;
}

/**
 * @brief ND03 Get Range Mode
 *        获取当前测量模式
 *
 * @param   pNxDevice 模组设备
 * @return  uint32_t 测量模式   
 * - ::ND03_DEVICEMODE_SINGLE_RANGING:单次测量
 * - ::ND03_DEVICEMODE_CONTINUOUS_RANGING:连续测量
 * - ::ND03_RANGE_MODE_FAST:快速模式
 * - ::ND03_RANGE_MODE_NORMAL:普通模式
 * - ::ND03_CALCU_MODE_SPOT:单点计算模式
 * - ::ND03_CALCU_MODE_AVER:平均计算模式
 */
uint32_t ND03_GetRangeMode(ND03_Dev_t *pNxDevice)
{
    return pNxDevice->config.range_mode;
}

/**
 * @brief ND03 Set Slave Address
 *        设置ND03的设备地址
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 配置ND03模组的i2c设备地址
 * @return int32_t
*/
int32_t ND03_SetSlaveAddr(ND03_Dev_t *pNxDevice, uint8_t addr)
{
	int32_t ret = ND03_ERROR_NONE;

	ret = ND03_WriteWord(pNxDevice, ND03_REG_SLAVE_ADDR, (uint32_t)addr);

    if(ret == ND03_ERROR_NONE)
	    pNxDevice->i2c_dev_addr = addr;

    return ret;
}

/** 
 * @brief ND03 Get Slave Address
 *        获取ND03的设备地址
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 获取的ND03模组i2c设备地址
 * @return int32_t
*/
int32_t ND03_GetSlaveAddr(ND03_Dev_t *pNxDevice, uint8_t *addr)
{
    int32_t ret = ND03_ERROR_NONE;

    *addr = pNxDevice->i2c_dev_addr;

    return ret;
}

/** 
 * @brief ND03 Set Modulation Frequency
 *        设置ND03的调制频率
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param freq: 调制频率
 * @return int32_t
*/
int32_t ND03_SetModFreq(ND03_Dev_t *pNxDevice, uint32_t freq)
{
    int32_t     ret = ND03_ERROR_NONE;

    if(pNxDevice->chip_info.nd03_fw_version >= ND03_FW_VERSION_V200_VALUE)
    {
        ret = ND03_WriteWord(pNxDevice, ND03_REG_MOD_FREQ, freq);
    }
    
    return ret;
}

/** 
 * @brief ND03 Get Modulation Frequency
 *        获取ND03的调制频率
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param freq: 调制频率
 * @return int32_t
*/
int32_t ND03_GetModFreq(ND03_Dev_t *pNxDevice, uint32_t *freq)
{
    int32_t     ret = ND03_ERROR_NONE;

    if(pNxDevice->chip_info.nd03_fw_version >= ND03_FW_VERSION_V200_VALUE)
    {
        ret = ND03_ReadWord(pNxDevice, ND03_REG_MOD_FREQ, freq);
    }

    return ret;
}


/**
 * @brief ND03 Set Offset State
 *        设置Offset补偿状态
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: ND03_ENABLE or ND03_DISABLE
 * @return int32_t
*/
int32_t ND03_SetOffsetState(ND03_Dev_t *pNxDevice, ND03_Status_e flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(flag != ND03_DISABLE)
        rbuf = rbuf | ND03_OFFSET_TOF_FLAG;
    else
        rbuf = rbuf & (~ND03_OFFSET_TOF_FLAG);

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_TOF_FLAG, rbuf); 

    return ret;
}

/**
 * @brief ND03 Get Offset State
 *        获取Offset补偿状态
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: 获取到的Offset状态变量指针
 * @return int32_t
*/
int32_t ND03_GetOffsetState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret = ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(ND03_ERROR_NONE == ret)
    {
        *flag = ((rbuf & ND03_OFFSET_TOF_FLAG) == ND03_OFFSET_TOF_FLAG) ? ND03_ENABLE : ND03_DISABLE;
    }
    
    return ret;
}

/**
 * @brief ND03 Set Auto Exposure State
 *        设置自动曝光状态
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: ND03_ENABLE or ND03_DISABLE
 * @return int32_t
*/
int32_t ND03_SetAutoExpState(ND03_Dev_t *pNxDevice, ND03_Status_e flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t     rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(flag != ND03_DISABLE)
        rbuf = rbuf | ND03_AE_TOF_FLAG;
    else
        rbuf = rbuf & (~ND03_AE_TOF_FLAG);

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_TOF_FLAG, rbuf); 

    return ret;
}

/**
 * @brief ND03 Get Auto Exposure State
 *        获取自动曝光状态
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: 获取到的ND03自动曝光状态变量指针
 * @return int32_t
*/
int32_t ND03_GetAutoExpState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t     rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(ND03_ERROR_NONE == ret)
    {
        *flag = ((rbuf & ND03_AE_TOF_FLAG) == ND03_AE_TOF_FLAG) ? ND03_ENABLE : ND03_DISABLE;
    }

    return ret;
}

/**
 * @brief ND03 Set Cross Talk State
 *        设置串扰矫正状态
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: ND03_ENABLE or ND03_DISABLE
 * @return int32_t
*/
int32_t ND03_SetXTalkState(ND03_Dev_t *pNxDevice, ND03_Status_e flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(flag != ND03_DISABLE)
        rbuf = rbuf | ND03_XT_TOF_FLAG;
    else
        rbuf = rbuf & (~ND03_XT_TOF_FLAG);

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_TOF_FLAG, rbuf); 

    return ret;
}

/**
 * @brief ND03 Get Cross Talk State
 *        获取串扰矫正状态
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: 获取到的ND03串扰矫正状态变量指针
 * @return int32_t
*/
int32_t ND03_GetXTalkState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(ND03_ERROR_NONE == ret)
    {
        *flag = ((rbuf & ND03_XT_TOF_FLAG) == ND03_XT_TOF_FLAG) ? ND03_ENABLE : ND03_DISABLE;
    }
    
    return ret;
}

/**
 * @brief ND03 Set Therm Compensation State
 *        设置温度补偿状态
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: ND03_ENABLE or ND03_DISABLE
 * @return int32_t
*/
int32_t ND03_SetThermCompensationState(ND03_Dev_t *pNxDevice, ND03_Status_e flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t     rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(flag != ND03_DISABLE)
        rbuf = rbuf | ND03_THERM_TOF_FLAG;
    else
        rbuf = rbuf & (~ND03_THERM_TOF_FLAG);

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_TOF_FLAG, rbuf); 

    return ret;
}

/**
 * @brief ND03 Get Therm Compensation State
 *        获取温度补偿状态
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param flag: 获取到的ND03温度矫正状态变量指针
 * @return int32_t
*/
int32_t ND03_GetThermCompensationState(ND03_Dev_t *pNxDevice, ND03_Status_e *flag)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);

    if(ND03_ERROR_NONE == ret)
    {
        *flag = ((rbuf & ND03_THERM_TOF_FLAG) == ND03_THERM_TOF_FLAG) ? ND03_ENABLE : ND03_DISABLE;
    }
    
    return ret;
}

/**
 * @brief ND03 Set GPIO Configuration
 *        设置GPIO的功能配置
 * @param pNxDevice: ND03模组设备信息结构体
 * @param functionality: GPIO功能设置
 *            ND03_GPIO_FUNCTIONALITY_OFF: 无功能（默认）
 *            ND03_GPIO_THRESHOLD_LOW：比较功能，当深度小于低阈值时，GPIO输出有效电平(只支持V2.0.0及以上版本)
 *            ND03_GPIO_THRESHOLD_HIGH：比较功能，当深度大于高阈值时，GPIO输出有效电平(只支持V2.0.0及以上版本)
 *            ND03_GPIO_THRESHOLD_DOMAIN_OUT：比较功能，当深度小于低阈值或者大于高阈值时，GPIO输出有效电平(只支持V2.0.0及以上版本)
 *            ND03_GPIO_NEW_MEASURE_READY:深度数据有效功能，当深度数据有效时，GPIO输出有效电平
 * @param polarity: GPIO有效电平(只支持V2.0.0及以上版本)
 *            ND03_GPIO_POLARITY_LOW：低电平有效
 *            ND03_GPIO_POLARITY_HIGH：高电平有效
 * @return int32_t
*/
int32_t ND03_SetGpioConfig(ND03_Dev_t *pNxDevice, ND03_GpioFunc_t functionality, ND03_GpioPolar_t polarity)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_GPIO_SETTING, &rbuf);
    /** 设置中断功能 */
    rbuf = rbuf & (~ND03_GPIO_FUNCTIONALITY_MASK);
    rbuf = rbuf | (functionality & ND03_GPIO_FUNCTIONALITY_MASK);
    /** 设置中断引脚极性 */
    if(polarity == ND03_GPIO_POLARITY_HIGH)
        rbuf = rbuf | ND03_GPIO_POLARITY_MASK;
    else
        rbuf = rbuf & (~ND03_GPIO_POLARITY_MASK);

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_GPIO_SETTING, rbuf);

    return ret;
}

/** 
 * @brief ND03 Get Gpio Config
 *        获取GPIO的功能配置
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param functionality: 获取到的gpio功能变量指针
 * @param polarity: 获取到的gpio极性变量指针(只支持V2.0.0及以上版本)
 * @return int32_t
*/
int32_t ND03_GetGpioConfig(ND03_Dev_t *pNxDevice, ND03_GpioFunc_t *functionality, ND03_GpioPolar_t *polarity)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_GPIO_SETTING, &rbuf);

    if(ND03_ERROR_NONE == ret)
    {
        // Polarity
        *polarity = ((rbuf & ND03_GPIO_POLARITY_MASK) == ND03_GPIO_POLARITY_MASK) ? ND03_GPIO_POLARITY_HIGH : ND03_GPIO_POLARITY_LOW;
        // Functionality
        *functionality = rbuf & ND03_GPIO_FUNCTIONALITY_MASK;
    }
    
    return ret;
}

/**
 * @brief ND03 Set Maximum Exposure 
 *        设置最大积分时间
 * @param pNxDevice: ND03模组设备信息结构体
 * @param max_exposure_us:  最大积分时间 / us
 * @return int32_t
*/
int32_t ND03_SetMaxExposure(ND03_Dev_t *pNxDevice, uint16_t max_exposure_us)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret = ND03_WriteWord(pNxDevice, ND03_REG_MAX_EXPOSURE, (uint32_t)max_exposure_us);

    return ret;
}

/**
 * @brief ND03 Get Maximum Exposure 
 *        获取最大积分时间的值
 * @param pNxDevice: ND03模组设备信息结构体
 * @param max_exposure_us:  获取的最大积分时间指针 / us
 * @return int32_t
*/
int32_t ND03_GetMaxExposure(ND03_Dev_t *pNxDevice, uint16_t *max_exposure_us)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret = ND03_ReadWord(pNxDevice, ND03_REG_MAX_EXPOSURE, &rbuf);
    if(ND03_ERROR_NONE == ret)
    {
        *max_exposure_us = (uint16_t)rbuf;
    }

    return ret;
}

/**
 * @brief ND03 Set Current Exposure
 *        设置当前ND03的积分时间
 * @param   pNxDevice   模组设备
 * @param   current_exp 当前积分时间/us
 * @return  uint32_t    
 */
int32_t ND03_SetCurrentExp(ND03_Dev_t *pNxDevice, uint16_t current_exp)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret = ND03_ReadWord(pNxDevice, ND03_REG_EXP_THERM, &rbuf);

    if(ND03_ERROR_NONE == ret){
        rbuf &= 0x0000FFFF;
        rbuf |= ((uint32_t)current_exp << 16);
        ret = ND03_WriteWord(pNxDevice, ND03_REG_EXP_THERM, rbuf);
    }

	return ret;
}

/**
 * @brief ND03 Get Current Exposure
 *        获取当前ND03的积分时间
 * @param   pNxDevice   模组设备
 * @return  uint32_t    当前积分时间/us
 */
uint16_t ND03_GetCurrentExp(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;
    uint16_t    current_exp = 0;

    ret = ND03_ReadWord(pNxDevice, ND03_REG_EXP_THERM, &rbuf);
    if(ND03_ERROR_NONE == ret){
        current_exp = (rbuf >> 16) & 0xFFFF;
    }

	return current_exp;
}

/**
 * @brief ND03 Get Current Therm
 *        获取当前ND03的温度
 * @param   pNxDevice   模组设备
 * @return  int32_t     当前温度/0.1度
 */
int16_t ND03_GetCurrentTherm(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;
    int16_t     current_therm = 0;

    ret = ND03_ReadWord(pNxDevice, ND03_REG_EXP_THERM, &rbuf);
    if(ND03_ERROR_NONE == ret){
        current_therm = rbuf & 0xFFFF;
    }

	return current_therm;
}

/**
 * @brief ND03 Hardware Sleep
 *        硬件待机
 * @param   pNxDevice   模组设备
 * @return  void  
 * 
 * @deprecated 由于特殊的原因，这个函数可能会在将来的版本取消，请使用ND03_Sleep函数进行替换 
 */
void ND03_HardwareSleep(ND03_Dev_t *pNxDevice)
{
    ND03_SetXShutPinLevel(pNxDevice, 0);
} 

/**
 * @brief ND03 Hardware Wakeup
 *        从硬件待机中唤醒
 * @param   pNxDevice   模组设备
 * @return  void  
 * 
 * @deprecated 由于特殊的原因，这个函数可能会在将来的版本取消，请使用ND03_Wakeup函数进行替换 
 */
void ND03_HardwareWakeup(ND03_Dev_t *pNxDevice)
{
    ND03_SetXShutPinLevel(pNxDevice, 1);
} 

/**
 * @brief ND03 Software Sleep
 *        软件待机
 * @param pNxDevice: ND03模组设备信息结构体
 * @return int32_t
 * 
 * @deprecated 由于特殊的原因，这个函数可能会在将来的版本取消，请使用ND03_Sleep函数进行替换 
*/
int32_t ND03_SoftwareSleep(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_SLEEP_SOFTWARE, ND03_SLEEP_SOFTWARE_VALUE);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_SETTING, ND03_SETTING_SW_SLEEP_MASK);

    return ret;
}

/**
 * @brief ND03 Software Wakeup
 *        从软件待机中唤醒
 * @param pNxDevice: ND03模组设备信息结构体
 * @return int32_t
 * 
 * @deprecated 由于特殊的原因，这个函数可能会在将来的版本取消，请使用ND03_Wakeup函数进行替换 
*/
int32_t ND03_SoftwareWakeup(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_SETTING, &rbuf);

    return ret;
}

/**
 * @brief ND03 Flash Lock
 *        ND03 Flash锁
 * @param pNxDevice 
 * @param enable    0: 不锁住， ！0: 锁住，不能读写
 * @return int32_t 
 */
int32_t ND03_FlashLock(ND03_Dev_t *pNxDevice, int32_t enable)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint16_t    cmd_address[] = {0x0888, 0x8430, 0x1010, 0x8430, 0x8460, 0x1010, 0x1010, 0x1010};
    uint32_t    cmd1_buffer[] = {0x00000001, 0x10000000, 0x20070600, 0x90000001};
    uint32_t    cmd2_buffer[] = {0x1c000430, 0x00000010, 0x00000014};
    uint32_t    cmd3_buffer[] = {0x00000002, 0x10000000, 0x20070100, 0x6000000f, 0x90000001};
    uint32_t    cmd4_buffer[] = {0x00000000};
    uint32_t    cmd5_buffer[] = {0x1c000430, 0x00000010, 0x00000014};
    uint32_t    cmd6_buffer[] = {0x1c000460, 0x00000001, 0x00000014};
    uint32_t    cmd7_buffer[] = {0x1c000440, 0x00000004, 0x00000014};


    if(pNxDevice->chip_info.nd03_fw_version <= ND03_FW_VERSION_V103_VALUE)
    {
        cmd4_buffer[0] = (enable != 0) ? 0x0000007C : 0x00000000;
        ret |= ND03_WriteWord  (pNxDevice, cmd_address[0], 0x03);
        ret |= ND03_WriteNWords(pNxDevice, cmd_address[1], cmd1_buffer, sizeof(cmd1_buffer)/sizeof(cmd1_buffer[0]));
        ret |= ND03_WriteNWords(pNxDevice, cmd_address[2], cmd2_buffer, sizeof(cmd2_buffer)/sizeof(cmd2_buffer[0]));
        ret |= ND03_WriteNWords(pNxDevice, cmd_address[3], cmd3_buffer, sizeof(cmd3_buffer)/sizeof(cmd3_buffer[0]));
        ret |= ND03_WriteNWords(pNxDevice, cmd_address[4], cmd4_buffer, sizeof(cmd4_buffer)/sizeof(cmd4_buffer[0]));
        ret |= ND03_WriteNWords(pNxDevice, cmd_address[5], cmd5_buffer, sizeof(cmd5_buffer)/sizeof(cmd5_buffer[0]));
        ret |= ND03_WriteNWords(pNxDevice, cmd_address[6], cmd6_buffer, sizeof(cmd6_buffer)/sizeof(cmd6_buffer[0]));
        ret |= ND03_WriteNWords(pNxDevice, cmd_address[7], cmd7_buffer, sizeof(cmd7_buffer)/sizeof(cmd7_buffer[0]));
    }
    return ret;
}

/**
 * @brief   ND03 Flash Power Down
 *          ND03的flash进入低功耗，功耗进一步降低
 * @param   pNxDevice: ND03模组设备信息结构体
 * @return  int32_t 
 */
int32_t ND03_FlashPowerDown(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint16_t    cmd_addr[3] = {0x8600, 0x0888, 0x1010};
    uint32_t    cmd_buffer[4] = {0x00000302, 0x10000000, 0x2007B900, 0x90000001};
    uint32_t    data_buffer[3] = {0x1C002600, 0x00000010, 0x00000014};

    NX_PRINTF("%s Start!\r\n", __func__);

    if(pNxDevice->chip_info.nd03_fw_version <= ND03_FW_VERSION_V103_VALUE){
        cmd_addr[0] = 0x8430;
        data_buffer[0] = 0x1C000430;
    }
    ret |= ND03_WriteNWords(pNxDevice, cmd_addr[0], cmd_buffer, sizeof(cmd_buffer)/sizeof(uint32_t));
    ret |= ND03_WriteWord(pNxDevice, cmd_addr[1], 0x03);
    ret |= ND03_WriteNWords(pNxDevice, cmd_addr[2], data_buffer, sizeof(data_buffer)/sizeof(uint32_t));
    ND03_Delay10us(20);

    NX_PRINTF("%s End!\r\n", __func__);

    return ret;
}

/**
 * @brief   ND03 Flash Power Wake Up
 *          ND03的flash从低功耗中唤醒
 * @param   pNxDevice: ND03模组设备信息结构体
 * @return  int32_t
 */
int32_t ND03_FlashPowerWakeup(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint16_t    cmd_addr[3] = {0xA600, 0x0888, 0x1010};
    uint32_t    cmd_buffer[4] = {0x00000302, 0x10000000, 0x2007ab00, 0x90000001};
    uint32_t    data_buffer[3] = {0x1c002600, 0x00000010, 0x00000014};

    NX_PRINTF("%s Start!\r\n", __func__);

	ND03_Delay10us(50);
    ND03_WriteByte(pNxDevice, ND03_REG_BOOT1, ND03_REG_BOOT1_VALUE);
    ND03_WriteWord(pNxDevice, ND03_REG_BOOT2, ND03_REG_BOOT2_VALUE);
	ND03_Delay10us(30);
	ND03_WriteWord(pNxDevice, ND03_REG_BOOT3, ND03_REG_BOOT3_VALUE);
	ND03_WriteWord(pNxDevice, ND03_REG_BOOT4, ND03_REG_BOOT4_VALUE);

    ret |= ND03_WriteNWords(pNxDevice, cmd_addr[0], cmd_buffer, sizeof(cmd_buffer)/sizeof(uint32_t));
    ret |= ND03_WriteWord(pNxDevice, cmd_addr[1], 0x03);
    ret |= ND03_WriteNWords(pNxDevice, cmd_addr[2], data_buffer, sizeof(data_buffer)/sizeof(uint32_t));
	ND03_Delay1ms(1);
	ret |= ND03_WriteWord(pNxDevice, ND03_REG_STATE, ND03_STATE_IDLE);
	ret |= ND03_WriteWord(pNxDevice, ND03_REG_BOOT5, ND03_REG_BOOT5_VALUE);

    NX_PRINTF("%s End!\r\n", __func__);

    return ret;
}

/**
 * @brief ND03 Sleep
 *        ND03待机
 * @param pNxDevice: ND03模组设备信息结构体
 * @return int32_t
*/
int32_t ND03_Sleep(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    if(pNxDevice->SetXShutPinLevelFunc != NULL)
        ND03_SetXShutPinLevel(pNxDevice, 0);
    else{
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_SLEEP_SOFTWARE, ND03_SLEEP_SOFTWARE_VALUE);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_SETTING, ND03_SETTING_SW_SLEEP_MASK);
    }

    return ret;
}

/**
 * @brief ND03 Wakeup
 *        唤醒ND03
 * @param pNxDevice: ND03模组设备信息结构体
 * @return int32_t
*/
int32_t ND03_Wakeup(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    if(pNxDevice->SetXShutPinLevelFunc != NULL)
        ND03_SetXShutPinLevel(pNxDevice, 1);
    else
        ret |= ND03_ReadWord(pNxDevice, ND03_REG_SETTING, &rbuf);
    ret |= ND03_WaitDeviceBootUp(pNxDevice);
    ret |= ND03_InitDevice(pNxDevice);

    return ret;
}

/**
 * @brief ND03 Deep Power Sleep
 *        ND03深度休眠
 * @param pNxDevice: ND03模组设备信息结构体
 * @return int32_t
*/
int32_t ND03_DeepPowerSleep(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    ND03_FlashPowerDown(pNxDevice);
    if(pNxDevice->SetXShutPinLevelFunc != NULL)
        ND03_SetXShutPinLevel(pNxDevice, 0);
    else{
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_SLEEP_SOFTWARE, ND03_SLEEP_SOFTWARE_VALUE);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_SETTING, ND03_SETTING_SW_SLEEP_MASK);
    }

    return ret;
}

/**
 * @brief ND03 Deep Power Wakeup
 *        深度休眠唤醒ND03
 * @param pNxDevice: ND03模组设备信息结构体
 * @return int32_t
*/
int32_t ND03_DeepPowerWakeup(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    if(pNxDevice->SetXShutPinLevelFunc != NULL)
        ND03_SetXShutPinLevel(pNxDevice, 1);
    else
        ret |= ND03_ReadWord(pNxDevice, ND03_REG_SETTING, &rbuf);
    ret |= ND03_FlashPowerWakeup(pNxDevice);
    ret |= ND03_WaitDeviceBootUp(pNxDevice);
    ret |= ND03_InitDevice(pNxDevice);

    return ret;
}

/**
 * @brief   ND03 Set Normal Therm Coe 
 *          ND03设置普通温度系数
 * @param   pNxDevice: ND03模组设备信息结构体
 * @param   therm_coe_normal: 温度系数值
 * @return  int32_t
 */
int32_t ND03_SetNormalThermCoe(ND03_Dev_t *pNxDevice, int32_t therm_coe_normal)
{
	int32_t         ret = ND03_ERROR_NONE;
	
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_THERM_COE_NORMAL, therm_coe_normal);
	
	return ret;
}

/**
 * @brief   ND03 Set Fast Therm Coe 
 *          ND03设置快速温度系数
 * @param   pNxDevice: ND03模组设备信息结构体
 * @param   therm_coe_normal: 温度系数值
 * @return  int32_t
 */
int32_t ND03_SetFastThermCoe(ND03_Dev_t *pNxDevice, int32_t therm_coe_fast)
{
	int32_t         ret = ND03_ERROR_NONE;

	ret |= ND03_WriteWord(pNxDevice, ND03_REG_THERM_COE_FAST, therm_coe_fast);

	return ret;
}

/**
 * @brief   ND03 Set Target Amp
 *          ND03设置目标幅度值
 * @param   pNxDevice: ND03模组设备信息结构体
 * @param   target_amp: 目标幅度值
 * @return  int32_t
 */
int32_t ND03_SetTargetAmp(ND03_Dev_t *pNxDevice, int32_t target_amp)
{
	int32_t         ret = ND03_ERROR_NONE;

    if(pNxDevice->chip_info.nd03_fw_version == ND03_FW_VERSION_V103_VALUE){
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_V103_TARGET_AMP, target_amp);
    }
    else{
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_TARGET_AMP, target_amp);
    }

	return ret;
}

/**
 * @brief   ND03 Set Target Amp Error
 *          ND03设置目标幅度容错值
 * @param   pNxDevice: ND03模组设备信息结构体
 * @param   target_amp_error: 目标幅度容错值
 * @return  int32_t
 */
int32_t ND03_SetTargetAmpError(ND03_Dev_t *pNxDevice, int32_t target_amp_error)
{
	int32_t         ret = ND03_ERROR_NONE;
	
    if(pNxDevice->chip_info.nd03_fw_version == ND03_FW_VERSION_V103_VALUE){
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_V103_TARGET_AMP_ERROR, target_amp_error);
    }
    else{
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_TARGET_AMP_ERROR, target_amp_error);
    }

	return ret;
}

/**
 * @brief ND03 InitDevice
 *        初始化设备
 * @param   pNxDevice   模组设备
 * @return  int32_t
 */
int32_t ND03_InitDevice(ND03_Dev_t *pNxDevice)
{
	int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    NX_PRINTF("ND03_InitDevice Start!\r\n");

    /** 获取设备信息 */
	ret |= ND03_GetDevInfo(pNxDevice);
    /** 停止测量 */
    ret |= ND03_StopMeasurement(pNxDevice);
    /** 关闭跨周期过滤 */
    ret |= ND03_ReadWord(pNxDevice, ND03_REG_TOF_FLAG, &rbuf);
    rbuf &= (~0x00000004);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_TOF_FLAG, rbuf);

    NX_PRINTF("ND03_InitDevice End!\r\n");

	return ret;
}

/**
 * @brief ND03 Get Firmware Version 
 *        获取ND03模组固件版本号
 * @param   pNxDevice   模组设备
 * @param   pFirmwareDataBuf  固件版本数据指针
 * @return  int32_t   
 */
int32_t ND03_GetFirmwareVersion(ND03_Dev_t *pNxDevice, uint32_t* pFirmwareDataBuf)
{
	int32_t     ret = ND03_ERROR_NONE;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_FW_VERSION, pFirmwareDataBuf);

	return ret;
}

/**
 * @brief ND03 Get Data from ND03 Flash
 *        获取ND03模组中flash数据
 * @param   pNxDevice   模组设备
 * @param   flash_addr: flash地址
 * @param   flash_rdata: 存放flash数据指针的指针
 * @return  int32_t
*/
int32_t ND03_GetFlashData(ND03_Dev_t *pNxDevice, uint32_t flash_addr, uint32_t* flash_rdata)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;
    uint16_t    retry_cnt = 500;

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_FLASH_ADDR, flash_addr);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, ND03_FLASH_DATA_REQ_MASK);
    while(retry_cnt)
    {
        ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &rbuf);
        if(ND03_FLASH_DATA_VAL_MASK == (rbuf & ND03_FLASH_DATA_VAL_MASK))
            break;
		ND03_Delay1ms(1);
        retry_cnt--;
	}

    if(retry_cnt == 0)
    {
        return ND03_ERROR_TIME_OUT;
    }

    /* 清除标志位 */
    ret |= ND03_ClearDataValidFlag(pNxDevice);
    ret |= ND03_ReadWord(pNxDevice, ND03_REG_FLASH_DATA, flash_rdata);

    return ret;
}

/**
 * @brief ND03 Get Device Information 
 *        获取设备当前信息
 * @param   pNxDevice   模组设备
 * @return  int32_t   
 */
int32_t ND03_GetDevInfo(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    /** 读取ND03模组固件版本号 */
	ret |= ND03_GetFirmwareVersion(pNxDevice, &pNxDevice->chip_info.nd03_fw_version);
	NX_PRINTF("FwVersion: V%d.%d.%d\r\n", pNxDevice->chip_info.nd03_fw_version/10000, pNxDevice->chip_info.nd03_fw_version/100%100,pNxDevice->chip_info.nd03_fw_version%100);
    /** 读取测量模式 */
    ret |= ND03_ReadWord(pNxDevice, ND03_REG_RANGE_MODE, &pNxDevice->config.range_mode);
    
    return ret;
}

/**
 * @brief ND03 Wait for Device Boot Up 
 *        等待ND03模组启动
 * @param   pNxDevice   模组设备
 * @return  ND03_ERROR_NONE:成功
 *          ND03_ERROR_BOOT:启动失败--请检测模组是否焊接好，还有i2c地址与读写函数是否错误。
 *          ND03_ERROR_FW:固件不兼容--请与FAE联系，是否模组的固件与SDK不兼容。
 */
int32_t ND03_WaitDeviceBootUp(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    int32_t     try_times = 200;
    int32_t     slave_addr = pNxDevice->i2c_dev_addr;
    uint32_t    rbuf = 0xFFFFFFFF;

    NX_PRINTF("%s Start!\r\n", __func__);

    /* 重启 */
    ND03_SetXShutPinLevel(pNxDevice, 0);
	ND03_Delay10us(2);
    ND03_SetXShutPinLevel(pNxDevice, 1);
	ND03_Delay10us(50);

    if(0xFFFFFFFF != rbuf)
    {
        NX_PRINTF("Please redefine the size of the word(__WORD_SIZE__) in nd03_stdint.h file to %d\r\n", (int16_t)sizeof(uint32_t));
        return ND03_ERROR_WORD_SIZE;
    }

    /* 判断是否完成启动项配置 */
    pNxDevice->i2c_dev_addr = ND03_DEFAULT_SLAVE_ADDR;
    ND03_ReadWord(pNxDevice, ND03_REG_DEV_ID, &rbuf);
    NX_PRINTF("ND03_REG_DEVICE_ID: 0x%08x\r\n", rbuf);


    /* 进行启动项配置 */
    if(rbuf != ND03_REG_DEV_ID_VALUE)
    {
        ret = ND03_WriteByte(pNxDevice, ND03_REG_BOOT1, ND03_REG_BOOT1_VALUE);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_BOOT2, ND03_REG_BOOT2_VALUE);
        ND03_Delay10us(30);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_BOOT3, ND03_REG_BOOT3_VALUE);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_BOOT4, ND03_REG_BOOT4_VALUE);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_STATE, ND03_STATE_IDLE);
        ret |= ND03_WriteWord(pNxDevice, ND03_REG_BOOT5, ND03_REG_BOOT5_VALUE);
        if(ND03_ERROR_NONE != ret)
            return ND03_ERROR_I2C;
        ND03_Delay1ms(5);
    }

    /* 等待进入工作状态 */
    do{
        ND03_Delay10us(50);
		ret |= ND03_ReadWord(pNxDevice, ND03_REG_STATE, &pNxDevice->dev_pwr_state);
	}while((pNxDevice->dev_pwr_state != ND03_STATE_SOFTWARE_READY) && (pNxDevice->dev_pwr_state != ND03_STATE_GOT_DEPTH)   \
            && --try_times);

	if((ND03_ERROR_NONE != ret) || (0 == try_times))
    {
        NX_PRINTF("ND03 boot error\r\n");

        return ND03_ERROR_BOOT;
    }

    if(slave_addr != ND03_DEFAULT_SLAVE_ADDR)
        ND03_SetSlaveAddr(pNxDevice,slave_addr);
    
    NX_PRINTF("%s End!\r\n", __func__);

    return ND03_ERROR_NONE;
}


/**
 * @brief ND03 Reboot On Abnormal Error
 *        发生异常错误后重启设备，通常是由于IIC通信异常
 * @param   pNxDevice   模组设备
 * @return  int32_t
 */
int32_t ND03_RebootOnAbnormalError(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;
    uint32_t    ranging_mode;
    
    if(pNxDevice->chip_info.nd03_fw_version <= ND03_FW_VERSION_V103_VALUE){
        ret = ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &rbuf);
        if((rbuf & ND03_OFFSET_CALIBRATION_REQ_MASK) | 
            (rbuf & ND03_XTALK_CALIB_REQ_MASK) | 
            (rbuf & ND03_OFFSET_CALIBRATION_VAL_MASK) | 
            (rbuf & ND03_XTALK_CALIB_VAL_MASK) )
        {
            ranging_mode = pNxDevice->config.range_mode;
            ret |= ND03_WaitDeviceBootUp(pNxDevice);
            ret |= ND03_InitDevice(pNxDevice);
            ret |= ND03_SetRangeMode(pNxDevice, ranging_mode);
            if((ranging_mode & ND03_DEVICEMODE_MASK) != ND03_DEVICEMODE_CONTINUOUS_RANGING)
                ret |= ND03_StartMeasurement(pNxDevice);
        }
    }

    return ret;
}
