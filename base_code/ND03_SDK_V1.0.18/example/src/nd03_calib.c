/**
 * @file nd03_calib.c
 * @author tongsheng.tang
 * @brief ND03 Calibration functions
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
 * @brief ND03 Read Error Status 读取错误寄存器状态
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @return int32_t: 如果通信正常，则返回错误寄存器状态值
 */
int32_t ND03_ReadErrorStatus(ND03_Dev_t *pNxDevice){
    int32_t     ret = ND03_ERROR_NONE;
    int32_t     error;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_ERROR_FLAG, (uint32_t*)&error);
    if(ND03_ERROR_NONE == ret)
    {
        ret = error;
        NX_PRINTF("Error: %d\r\n", error);
    }

    return ret;
}


/**
 * @brief ND03 Offset Calibration Request 发送偏移量标定请求
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @return int32_t
 */
int32_t ND03_OffsetCalibrationRequest(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, ND03_OFFSET_CALIBRATION_REQ_MASK);

    return ret;
}


/**
 * @brief ND03 XTalkCalibration Request 发送串扰标定请求
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @return int32_t
 */
int32_t ND03_XTalkCalibrationRequest(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret |= ND03_WriteWord(pNxDevice, ND03_REG_DATA_VAL_REQ, ND03_XTALK_CALIB_REQ_MASK);

    return ret;
}


/**
 * @brief ND03 Wait for Calibration 等待标定的完成
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @return int32_t
 */
int32_t ND03_WaitforOffsetCalibration(ND03_Dev_t *pNxDevice)
{
	int32_t     ret = ND03_ERROR_NONE;
	uint32_t    rbuf = 0;
    uint16_t    retry_cnt = 50;
	
    while(retry_cnt)
    {
		ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &rbuf);
        if(ND03_OFFSET_CALIBRATION_VAL_MASK == rbuf)
            break;
		ND03_Delay1ms(500);
        retry_cnt--;
	}

    if(retry_cnt == 0)
    {
        NX_PRINTF("Calibration Error\r\n");
        return ND03_ERROR_TIME_OUT;
    }
    /* 清除标志位 */
    ret |= ND03_ClearDataValidFlag(pNxDevice);

    return ret;
}


/**
 * @brief ND03 Wait for XTalk Calibration 等待串扰标定的完成
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @return int32_t
 */
int32_t ND03_WaitforXTalkCalibration(ND03_Dev_t *pNxDevice)
{
	int32_t     ret = ND03_ERROR_NONE;
	uint32_t    rbuf = 0;
    uint16_t    retry_cnt = 50;

    while(retry_cnt)
    {
		ret |= ND03_ReadWord(pNxDevice, ND03_REG_DATA_VAL_REQ, &rbuf);
        if(ND03_XTALK_CALIB_VAL_MASK == rbuf)
            break;
		ND03_Delay1ms(500);
        retry_cnt--;
	}

    if(retry_cnt == 0)
    {
        return ND03_ERROR_TIME_OUT;
    }
    /* 清除标志位 */
    ret |= ND03_ClearDataValidFlag(pNxDevice);

    return ret;
}


/**
 * @brief   ToF Offset 标定
 * 
 * @param   pNxDevice   设备模组
 * @return  int32_t  
 * @retval  0:  成功
 * @retval  !0: Offset标定失败
 * 
 * @deprecated 由于特殊的原因，这个函数可能会在将来的版本取消，请使用ND03_OffsetCalibration函数进行替换          
 */
int32_t ND03_Calibration(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    NX_PRINTF("\r\n%s Start!\r\n", __func__);

    ret |= ND03_FlashLock(pNxDevice, 0);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0xFF00AA55);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x01020304);
    ret |= ND03_SetOffsetCalibDistMM(pNxDevice, 150);    /* 设置Offset标定距离(mm) */
    ret |= ND03_OffsetCalibrationRequest(pNxDevice);
    ND03_Delay1ms(1500);
    ret |= ND03_WaitforOffsetCalibration(pNxDevice);
    if(ND03_ERROR_NONE == ret)
        ret |= ND03_ReadErrorStatus(pNxDevice);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0x00000000);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x00000000);
    ret |= ND03_FlashLock(pNxDevice, 1);

    NX_PRINTF("%s End!\r\n", __func__);

    return ret;
}


/**
 * @brief   ToF Offset 标定
 * 
 * @param   pNxDevice   设备模组
 * @return  int32_t  
 * @retval  0:  成功
 * @retval  !0: Offset标定失败
 */
int32_t ND03_OffsetCalibration(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    NX_PRINTF("\r\n%s Start!\r\n", __func__);

    ret |= ND03_FlashLock(pNxDevice, 0);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0xFF00AA55);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x01020304);
    ret |= ND03_SetOffsetCalibDistMM(pNxDevice, 150);    /* 设置Offset标定距离(mm) */
    ret |= ND03_OffsetCalibrationRequest(pNxDevice);
    ND03_Delay1ms(1500);
    ret |= ND03_WaitforOffsetCalibration(pNxDevice);
    if(ND03_ERROR_NONE == ret)
        ret |= ND03_ReadErrorStatus(pNxDevice);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0x00000000);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x00000000);
    ret |= ND03_FlashLock(pNxDevice, 1);

    NX_PRINTF("%s End!\r\n", __func__);

    return ret;
}


/**
 * @brief    ToF Offset 标定
 * @details  可以指定标定距离
 * 
 * @param   pNxDevice       设备模组
 * @param   calib_depth_mm  标定距离
 * @return  int32_t  
 * @retval  0:  成功
 * @retval  !0: Offset标定失败
 * 
 * @deprecated 由于特殊的原因，这个函数可能会在将来的版本取消，请使用ND03_OffsetCalibrationAtDepth函数进行替换     
 */
int32_t ND03_CalibrationAtDepth(ND03_Dev_t *pNxDevice, uint16_t calib_depth_mm)
{
    int32_t     ret = ND03_ERROR_NONE;

    NX_PRINTF("\r\n%s Start!\r\n", __func__);

    ret |= ND03_FlashLock(pNxDevice, 0);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0xFF00AA55);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x01020304);
    ret |= ND03_SetOffsetCalibDistMM(pNxDevice, calib_depth_mm);    /* 设置Offset标定距离(mm) */
    ret |= ND03_OffsetCalibrationRequest(pNxDevice);
    ND03_Delay1ms(1500);
    ret |= ND03_WaitforOffsetCalibration(pNxDevice);
    if(ND03_ERROR_NONE == ret)
        ret |= ND03_ReadErrorStatus(pNxDevice);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0x00000000);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x00000000);
    ret |= ND03_FlashLock(pNxDevice, 1);

    NX_PRINTF("%s End!\r\n", __func__);

    return ret;
}


/**
 * @brief    ToF Offset 标定
 * @details  可以指定标定距离
 * 
 * @param   pNxDevice       设备模组
 * @param   calib_depth_mm  标定距离
 * @return  int32_t  
 * @retval  0:  成功
 * @retval  !0: Offset标定失败
 */
int32_t ND03_OffsetCalibrationAtDepth(ND03_Dev_t *pNxDevice, uint16_t calib_depth_mm)
{
    int32_t     ret = ND03_ERROR_NONE;

    NX_PRINTF("\r\n%s Start!\r\n", __func__);

    ret |= ND03_FlashLock(pNxDevice, 0);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0xFF00AA55);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x01020304);
    ret |= ND03_SetOffsetCalibDistMM(pNxDevice, calib_depth_mm);    /* 设置Offset标定距离(mm) */
    ret |= ND03_OffsetCalibrationRequest(pNxDevice);
    ND03_Delay1ms(1500);
    ret |= ND03_WaitforOffsetCalibration(pNxDevice);
    if(ND03_ERROR_NONE == ret)
        ret |= ND03_ReadErrorStatus(pNxDevice);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0x00000000);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x00000000);
    ret |= ND03_FlashLock(pNxDevice, 1);

    NX_PRINTF("%s End!\r\n", __func__);

    return ret;
}


/**
 * @brief 串扰标定功能
 * 
 * @note  矫正玻璃盖板串扰
 * 
 * @param   pNxDevice   模组设备       
 * @return  int32_t   
 * @retval  0:  成功
 * @retval  !0: 串扰标定失败
 */
int32_t ND03_XTalkCalibration(ND03_Dev_t *pNxDevice)
{
	int32_t     ret = ND03_ERROR_NONE;
    int32_t     cnt;
    ND03_RangingData_t  ranging_data;
    uint32_t    exp;
	
    NX_PRINTF("\r\n%s Start!\r\n", __func__);

    ret |= ND03_FlashLock(pNxDevice, 0);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0xFF00AA55);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x01020304);
    if(pNxDevice->chip_info.nd03_fw_version < ND03_FW_VERSION_V200_VALUE){
        ret |= ND03_SetCurrentExp(pNxDevice, 1000);
        ret |= ND03_ClearXTalkAmp(pNxDevice);
        for(cnt = 0; cnt < 10; cnt++){
            ret |= ND03_StartMeasurement(pNxDevice);
            ret |= ND03_GetRangingData(pNxDevice, &ranging_data);
            exp = (uint32_t)ND03_GetCurrentExp(pNxDevice);
            NX_PRINTF("cnt: %d, depth: %d, amp: %d, exp: %d\r\n", cnt, ranging_data.depth, ranging_data.amp, exp);
        }
        if(exp < 4800){
            exp = 1400*exp/ranging_data.amp + 50;
            do{
                exp -= 50;
                ret |= ND03_SetCurrentExp(pNxDevice, exp);
                ret |= ND03_StartMeasurement(pNxDevice);
                ret |= ND03_GetRangingData(pNxDevice, &ranging_data);
                NX_PRINTF("cnt: %d, depth: %d, amp: %d, exp: %d\r\n", cnt, ranging_data.depth, ranging_data.amp, exp);
            }while(ranging_data.amp > 800);
            ret |= ND03_SetMaxExposure(pNxDevice, exp);
        }
        switch(pNxDevice->chip_info.nd03_fw_version){
            case ND03_FW_VERSION_V102_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V102_CALIB_ERROR, 0x00000000); break;
            case ND03_FW_VERSION_V103_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V103_CALIB_ERROR, 0x00000000); break;
            case ND03_FW_VERSION_V104_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V104_CALIB_ERROR, 0x00000000); break;
            default: break;
        }
    }

    ret |= ND03_SetXTalkCalibDistMM(pNxDevice, 800);      /*! 设置串扰标定距离(mm) */
    ret |= ND03_XTalkCalibrationRequest(pNxDevice);
    ND03_Delay1ms(1500);
    ret |= ND03_WaitforXTalkCalibration(pNxDevice);
    if(ND03_ERROR_NONE == ret)
        ret |= ND03_ReadErrorStatus(pNxDevice);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0x00000000);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x00000000);
    ret |= ND03_FlashLock(pNxDevice, 1);

    NX_PRINTF("%s End!\r\n", __func__);
	
	return ret;
}


/**
 * @brief    串扰标定功能
 * @details  可以指定标定距离
 * 
 * @note  矫正玻璃盖板串扰
 * 
 * @param   pNxDevice               模组设备  
 * @param   xtalk_calib_depth_mm    标定距离    
 * @return  int32_t   
 * @retval  0:  成功
 * @retval  !0: 串扰标定失败
 */
int32_t ND03_XTalkCalibrationAtDepth(ND03_Dev_t *pNxDevice, uint16_t xtalk_calib_depth_mm)
{
	int32_t     ret = ND03_ERROR_NONE;
    int32_t     cnt;
    ND03_RangingData_t  ranging_data;
    uint32_t    exp;
    uint16_t    max_exp;
	
    NX_PRINTF("\r\n%s Start!\r\n", __func__);

    ret |= ND03_FlashLock(pNxDevice, 0);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0xFF00AA55);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x01020304);
    ND03_GetMaxExposure(pNxDevice, &max_exp);
    if(pNxDevice->chip_info.nd03_fw_version < ND03_FW_VERSION_V200_VALUE){
        ret |= ND03_SetCurrentExp(pNxDevice, 1000);
        ret |= ND03_ClearXTalkAmp(pNxDevice);
        for(cnt = 0; cnt < 10; cnt++){
            ret |= ND03_StartMeasurement(pNxDevice);
            ret |= ND03_GetRangingData(pNxDevice, &ranging_data);
            exp = (uint32_t)ND03_GetCurrentExp(pNxDevice);
            NX_PRINTF("cnt: %d, depth: %d, amp: %d, exp: %d\r\n", cnt, ranging_data.depth, ranging_data.amp, exp);
        }
        if(exp < 4800){
            exp = 1400*exp/ranging_data.amp + 50;
            do{
                exp -= 50;
                ret |= ND03_SetCurrentExp(pNxDevice, exp);
                ret |= ND03_StartMeasurement(pNxDevice);
                ret |= ND03_GetRangingData(pNxDevice, &ranging_data);
                NX_PRINTF("cnt: %d, depth: %d, amp: %d, exp: %d\r\n", cnt, ranging_data.depth, ranging_data.amp, exp);
            }while(ranging_data.amp > 1800);
            ret |= ND03_SetMaxExposure(pNxDevice, exp);
        }
        switch(pNxDevice->chip_info.nd03_fw_version){
            case ND03_FW_VERSION_V102_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V102_CALIB_ERROR, 0x00000000); break;
            case ND03_FW_VERSION_V103_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V103_CALIB_ERROR, 0x00000000); break;
            case ND03_FW_VERSION_V104_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V104_CALIB_ERROR, 0x00000000); break;
            default: break;
        }
    }

    ret |= ND03_SetXTalkCalibDistMM(pNxDevice, xtalk_calib_depth_mm);      /*! 设置串扰标定距离(mm) */
    ret |= ND03_XTalkCalibrationRequest(pNxDevice);
    ND03_Delay1ms(1500);
    ret |= ND03_WaitforXTalkCalibration(pNxDevice);
    if(ND03_ERROR_NONE == ret)
        ret |= ND03_ReadErrorStatus(pNxDevice);
    ND03_SetMaxExposure(pNxDevice, max_exp);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA1, 0x00000000);
    ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_ENA2, 0x00000000);
    ret |= ND03_FlashLock(pNxDevice, 1);

    NX_PRINTF("%s End!\r\n", __func__);
	
	return ret;
}


/**
 * @brief ND03 Clear XTalk Amp
          清除串扰幅度值，该方法断电不保存

 * @param pNxDevice: ND03模组设备信息结构体指针
 * @return int32_t
 * @retval  0:  运行成功
 * @retval  !0: 运行失败
*/
int32_t ND03_ClearXTalkAmp(ND03_Dev_t *pNxDevice)
{
    int32_t     ret = ND03_ERROR_NONE;

    switch(pNxDevice->chip_info.nd03_fw_version){
        case ND03_FW_VERSION_V102_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V102_XTALK_AMP, 0x00000000); break;
        case ND03_FW_VERSION_V103_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V103_XTALK_AMP, 0x00000000); break;
        case ND03_FW_VERSION_V104_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V104_XTALK_AMP, 0x00000000); break;
        default: ret |= ND03_WriteWord(pNxDevice, ND03_REG_XTALK_AMP, 0x00000000); break;
    }

    return ret;
}


/**
 * @brief ND03 Set XTalk Amp
 *        设置串扰幅度值，该方法断电不保存
 * 
 * @param   pNxDevice   模组设备   
 * @param   xtalk_amp   串扰幅度值
 * @return  int32_t   
 * @retval  0:  运行成功
 * @retval  !0: 运行失败
 */
int32_t ND03_SetXTalkAmp(ND03_Dev_t *pNxDevice, int32_t xtalk_amp)
{
    int32_t     ret = ND03_ERROR_NONE;

    switch(pNxDevice->chip_info.nd03_fw_version){
        case ND03_FW_VERSION_V102_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V102_XTALK_AMP, xtalk_amp*128); break;
        case ND03_FW_VERSION_V103_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V103_XTALK_AMP, xtalk_amp*128); break;
        case ND03_FW_VERSION_V104_VALUE: ret |= ND03_WriteWord(pNxDevice, ND03_REG_V104_XTALK_AMP, xtalk_amp*128); break;
        default: ret |= ND03_WriteWord(pNxDevice, ND03_REG_XTALK_AMP, xtalk_amp); break;
    }

    return ret;
}


/**
 * @brief ND03 Get XTalk Amp
 *        获取串扰幅度值
 * 
 * @note  用于判断串扰是否过大，从而优化盖板
 * 
 * @param   pNxDevice   模组设备   
 * @param   xtalk_amp   串扰幅度值指针
 * @return  int32_t   
 * @retval  0:  运行成功
 * @retval  !0: 运行失败
 */
int32_t ND03_GetXTalkAmp(ND03_Dev_t *pNxDevice, int32_t *xtalk_amp)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    xtalk_offset_depth;

    switch(pNxDevice->chip_info.nd03_fw_version){
        case ND03_FW_VERSION_V102_VALUE:
            ret |= ND03_ReadWord(pNxDevice, ND03_REG_V102_XTALK_AMP, (uint32_t*)xtalk_amp);
            *xtalk_amp = *xtalk_amp / 128;
            ret |= ND03_ReadWord(pNxDevice, ND03_REG_V102_XTALK_DEPTH , &xtalk_offset_depth);
            if(xtalk_offset_depth == ND03_DEPTH_LOW_AMP || xtalk_offset_depth == ND03_DEPTH_OVERFLOW)
            {
                ret = ND03_ERROR_XTALK_CALIBRATION;
                *xtalk_amp = ND03_DEPTH_OVERFLOW;
            }
            break;
        case ND03_FW_VERSION_V103_VALUE:
            ret |= ND03_ReadWord(pNxDevice, ND03_REG_V103_XTALK_AMP, (uint32_t*)xtalk_amp);
            *xtalk_amp = *xtalk_amp / 128;
            ret |= ND03_ReadWord(pNxDevice, ND03_REG_V103_XTALK_DEPTH , &xtalk_offset_depth);
            if(xtalk_offset_depth == ND03_DEPTH_LOW_AMP || xtalk_offset_depth == ND03_DEPTH_OVERFLOW)
            {
                ret = ND03_ERROR_XTALK_CALIBRATION;
                *xtalk_amp = ND03_DEPTH_OVERFLOW;
            }
            break;
        case ND03_REG_V104_XTALK_AMP:
            ret |= ND03_ReadWord(pNxDevice, ND03_REG_V104_XTALK_AMP, (uint32_t*)xtalk_amp);
            *xtalk_amp = *xtalk_amp / 128;
            if(ND03_ERROR_NONE == ret)
                ret |= ND03_ReadErrorStatus(pNxDevice);
            break;
        default:
            ret |= ND03_ReadWord(pNxDevice, ND03_REG_XTALK_AMP, (uint32_t*)xtalk_amp);
            if(ND03_ERROR_NONE == ret)
                ret |= ND03_ReadErrorStatus(pNxDevice);
            break;
    }

    return ret;
}


/**
 * @brief ND03 Set Offset Calibration Depth MM
 *        设置偏移量标定深度
 * @param   pNxDevice 模组设备
 * @param   depth_mm   偏移量标定深度 / mm
 * @return  int32_t   
 * @retval  0:  运行成功
 * @retval  !0: 运行失败
 */
int32_t ND03_SetOffsetCalibDistMM(ND03_Dev_t *pNxDevice, uint16_t depth_mm)
{
	int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;


    ret |= ND03_ReadWord(pNxDevice, ND03_REG_CALIB_XT_OFFSET, &rbuf);
    rbuf = (rbuf & 0xFFFF0000) + ((uint32_t)depth_mm & 0x0000FFFF);
	ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_XT_OFFSET, rbuf);

	return ret;
}


/**
 * @brief ND03 Set XTalk Calibration Depth MM
 *        设置串扰标定的深度
 * 
 * @note  建议取值范围在60~120cm，推荐使用80cm
 * 
 * @param   pNxDevice   模组设备
 * @param   depth_mm   串扰标定深度 / mm
 * @return  int32_t
 */
int32_t ND03_SetXTalkCalibDistMM(ND03_Dev_t *pNxDevice, uint16_t depth_mm)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint32_t    rbuf;

    ret |= ND03_ReadWord(pNxDevice, ND03_REG_CALIB_XT_OFFSET, &rbuf);
    rbuf = (rbuf & 0x0000FFFF) + (((uint32_t)depth_mm<<16) & 0xFFFF0000);
	ret |= ND03_WriteWord(pNxDevice, ND03_REG_CALIB_XT_OFFSET, rbuf);

	return ret;
}

