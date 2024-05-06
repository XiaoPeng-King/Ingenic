/**
 * @file nd03_data.c
 * @author tongsheng.tang
 * @brief ND03 get depth data functions
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



/**
 * @brief ND03 Wait Depth And Amp Buf Ready 
 *        等待ND03测距完成
 * @param   pNxDevice 模组设备
 * @return  int32_t  
 * @retval  函数执行结果
 * - 0：正常返回
 * - ::ND03_ERROR_TIME_OUT:获取数据超时
 */
int32_t ND03_WaitDepthAndAmpBufReady(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    buf_valid_flag = 0x0;
    uint32_t    i = 1000;

    while(i)
    {
        ret = ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &buf_valid_flag);
        if(ret != ND03_ERROR_NONE)
        {
            return ND03_ERROR_I2C;
        }
        if(ND03_DEPTH_DATA_VAL_MASK == (buf_valid_flag & ND03_DEPTH_DATA_VAL_MASK))
            break;
        ND03_Delay10us(20);
        i--;
    }

    if(i == 0)
    {
        return ND03_ERROR_TIME_OUT;
    }

    return ret;
}

/**
 * @brief ND03 Get Depth And Amp Buf Ready 
 *        检测ND03测距是否完成
 * @param   pNxDevice 模组设备
 * @return  int32_t  
 * @retval  函数执行结果
 * - 0：未完成
 *   1: 完成
 * - ::ND03_ERROR_I2C:IIC通讯错误/数据异常
 */
int32_t ND03_GetDepthAndAmpBufReady(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    buf_valid_flag = 0x0;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &buf_valid_flag);
    if(ND03_ERROR_NONE != ret)
    {
		NX_PRINTF("ND03_ReadWord failed!!!\r\n");
        return ret;
    }
    // NX_PRINTF("%s -> buf_valid_flag2: 0x%08x\r\n", __func__, buf_valid_flag);
    if(ND03_DEPTH_DATA_VAL_MASK == (buf_valid_flag & ND03_DEPTH_DATA_VAL_MASK))
        return 1;

    return ret;
}

/**
 * @brief ND03 Start Measurement 
 *        发送开始测量信号
 * @param   pNxDevice 模组设备
 * @return  int32_t   
 */
int32_t ND03_StartMeasurement(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    /* 异常错误重启 */
    ret |= ND03_RebootOnAbnormalError(pNxDevice);
    /* 发送触发信号 */
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, ND03_DEPTH_DATA_REQ_MASK);

    return ret;
}

/**
 * @brief ND03 Stop Measurement 
 *        发送结束测量信号，用于连续模式
 * @param   pNxDevice 模组设备
 * @return  int32_t   
 */
int32_t ND03_StopMeasurement(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    
    /* 关闭连续模式 */
    ret |= ND03_SetRangeMode(pNxDevice, ND03_GetRangeMode(pNxDevice) & (~ND03_DEVICEMODE_CONTINUOUS_RANGING));
    /* 清除标志位 */
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, 0x00000000);

    return ret;
}

/**
 * @brief ND03 Read Depth And Amp Data 
 *        读取ND03寄存器获取深度幅度数据，数据更新于一次测距完成后
 * @param   pNxDevice 模组设备
 * @param   pData  获取到的深度和幅度数据
 * @return  int32_t   
 */
int32_t ND03_ReadDepthAndAmpData(ND03_Dev_t *pNxDevice, ND03_RangingData_t *pData)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;
    uint32_t    depth;
    uint32_t    amp;

    if(pNxDevice->chip_info.nd03_fw_version == ND03_FW_VERSION_V102_VALUE)
    {
        ret |= ND03_ReadWord(pNxDevice, ND03_REG_V102_DEPTH, &depth);
        ret |= ND03_ReadWord(pNxDevice, ND03_REG_V102_AMP, &amp);
    }
    else if(pNxDevice->chip_info.nd03_fw_version == ND03_FW_VERSION_V103_VALUE)
    {
        ret |= ND03_ReadWord(pNxDevice, ND03_REG_V103_DEPTH, &depth);
        ret |= ND03_ReadWord(pNxDevice, ND03_REG_V103_AMP, &amp);
    }
    else
    {
        ret |= ND03_ReadWord(pNxDevice, ND03_REG_AMP_DEPTH, &rbuf);
        amp = rbuf >> 16;
        depth = rbuf & 0xFFFF;
    }

    if((depth != ND03_DEPTH_LOW_AMP) && (depth != ND03_DEPTH_OVERFLOW) && ((amp < 30) || (depth == ND03_DEPTH_FILTER)))
    {
        depth = ND03_DEPTH_LOW_AMP;
    }

    pData->depth = (uint16_t)depth;
    pData->amp = (uint16_t)amp;

    return ret;
}

/**
 * @brief ND03 Clear Data Valid Flag 
 *        清除ND03测量数据的有效位，取完一次数
 *        据做的操作，通知ND03数据已读取
 * @param   pNxDevice 模组设备
 * @return  int32_t   
 */
int32_t ND03_ClearDataValidFlag(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf = 0;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &rbuf);
    rbuf = rbuf & 0x00000001;
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, rbuf);

    return ret;
}

/**
 * @brief ND03 Get Ranging Data 
 *        从ND03中获取一次深度数据，
 *        需要与ND03_StartMeasurement函数搭配
 * @param   pNxDevice 模组设备
 * @param   pData  获取到的深度和幅度数据
 * @return  int32_t   
 */
int32_t ND03_GetRangingData(ND03_Dev_t *pNxDevice, ND03_RangingData_t *pData)
{
    int32_t     ret = ND03_ERROR_NONE;

    /* 异常错误重启 */
    ret |= ND03_RebootOnAbnormalError(pNxDevice);
    /* 等待测量完成 */
    ret |= ND03_WaitDepthAndAmpBufReady(pNxDevice);
    /* 读取测量数据 */
    ret |= ND03_ReadDepthAndAmpData(pNxDevice, pData);
    /* 清除数据有效标志位 */
    ret |= ND03_ClearDataValidFlag(pNxDevice);
    /* 异常错误重启 */
    ret |= ND03_RebootOnAbnormalError(pNxDevice);

    return ret;
}


