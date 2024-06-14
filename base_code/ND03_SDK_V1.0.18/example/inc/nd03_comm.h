/**
 * @file nd03_comm.h
 * @author tongsheng.tang
 * @brief ND03 communication functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */
#ifndef __ND03_COMM_H__
#define __ND03_COMM_H__

#include "nd03_def.h"

/** @defgroup ND03_Communication_Group ND03 Communication Funtions
 *  @brief ND03 Communication Funtions
 *  @{
 */


/** 
 * @struct ND03_Func_Ptr_t
 * 
 * @brief 链接平台的API\n
 */
typedef struct{
	int32_t(*I2C_WriteNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t); ///< i2c写函数
	int32_t(*I2C_ReadNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t); ///< i2c读函数
	void(*Delay1msFunc)(uint32_t );   ///< 延时1ms函数
	void(*Delay10usFunc)(uint32_t );  ///< 延时10us函数
}ND03_Func_Ptr_t;


/** 初始化ND03库的API */
void ND03_RegisteredPlatformApi(ND03_Func_Ptr_t dev_op);

/** IIC注册函数，即将实现的iic接口函数传输注册到本SDK中 */
void ND03_RegisteredIICApi(
        int32_t(*I2C_WriteNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t len),
        int32_t(*I2C_ReadNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t len)
    );

/** 延时时间（ms） */
void ND03_Delay1ms(uint32_t ms);

/** 延时时间（10us） */
void ND03_Delay10us(uint32_t us);

/** 设置xshut引脚的电平 */
void ND03_SetXShutPinLevel(ND03_Dev_t *pNxDevice, uint32_t level);
    
/** 对ND03寄存器写1个字节 */
int32_t ND03_WriteByte(ND03_Dev_t *pNxDevice, uint16_t addr, uint8_t wdata);

/** 对ND03寄存器写1个字 */
int32_t ND03_WriteWord(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t wdata);

/** 对ND03寄存器读1个字 */
int32_t ND03_ReadWord(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata);

/** 对ND03寄存器写N个字 */
int32_t ND03_WriteNWords(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *wdata, uint32_t len);

/** 对ND03寄存器读N个字 */
int32_t ND03_ReadNWords(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata, uint32_t len);


/** @} ND03_Communication_Group */

#endif

