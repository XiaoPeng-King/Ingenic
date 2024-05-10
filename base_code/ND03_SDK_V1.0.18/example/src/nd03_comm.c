/**
 * @file nd03_comm.c
 * @author tongsheng.tang
 * @brief ND03 communication functions
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */

#include "nd03_stdint.h"
#include "nd03_comm.h"
#include "nd03_dev.h"
#include "nd03_data.h"
#include "nd03_calib.h"


#if ND03_PLATFORM != PLATFORM_C51
//Function ptr
/** iic单字节写函数指针 */
int32_t(*I2C_WriteNBytesLib)(uint8_t, uint16_t, uint8_t *, uint8_t);
/** iic单字节读函数指针 */
int32_t(*I2C_ReadNBytesLib)(uint8_t, uint16_t,uint8_t *, uint8_t);
/** 延时1ms函数指针 */
void(*Delay1msLib)(uint32_t );
/** 延时10us函数指针 */
void(*Delay10usLib)(uint32_t );
#else
#include "nd03_platform.h"

#define I2C_WriteNBytesLib      i2c_write_nbytes
#define I2C_ReadNBytesLib       i2c_read_nbytes
#define Delay1msLib             delay_1ms
#define Delay10usLib            delay_10us
#define SetXshutPinLevelLib     set_xshut_pin_level
#endif


/**
 * @brief ND03 Registered Platform Api
 *        初始化ND03库的API
 * @param   dev_op   ND03库的API入口地址
 * @return  void   
 */
void ND03_RegisteredPlatformApi(ND03_Func_Ptr_t dev_op)
{
#if ND03_PLATFORM != PLATFORM_C51
    // ND03_RegisteredIICApi(dev_op.I2C_WriteNBytesFunc, dev_op.I2C_ReadNBytesFunc);
    I2C_WriteNBytesLib = dev_op.I2C_WriteNBytesFunc;
    I2C_ReadNBytesLib = dev_op.I2C_ReadNBytesFunc;
    Delay1msLib = dev_op.Delay1msFunc;
    Delay10usLib = dev_op.Delay10usFunc;
#endif
}


/**
 * @brief 注册IIC API
 *
 * @param   I2C_WriteNBytesFunc iic 写N个bytes函数
 * @param   I2C_ReadNBytesFunc  iic 读N个bytes函数
 * @return  void
 */
void ND03_RegisteredIICApi(
        int32_t(*I2C_WriteNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t),
        int32_t(*I2C_ReadNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t)
    )
{
#if ND03_PLATFORM != PLATFORM_C51
    I2C_ReadNBytesLib = I2C_ReadNBytesFunc;
    I2C_WriteNBytesLib = I2C_WriteNBytesFunc;
#endif
}

/**
 * @brief ND03 Delay 1ms
 * @param   ms 延时时间
 * @return  void  
 */
void ND03_Delay1ms(uint32_t ms)
{
    Delay1msLib(ms);
} 

/**
 * @brief ND03 Delay 10us
 * @param   us 延时时间
 * @return  void  
 */
void ND03_Delay10us(uint32_t us)
{
    Delay10usLib(us);
} 

/**
 * @brief ND03 Set XShut Pin Level
 *        设置xshut引脚的电平
 * @param   pNxDevice   模组设备
 * @param   level    xshut引脚电平，0为低电平，1为高电平
 * @return  void  
 */
void ND03_SetXShutPinLevel(ND03_Dev_t *pNxDevice, uint32_t level)
{
#if ND03_PLATFORM != PLATFORM_C51
    if(pNxDevice->SetXShutPinLevelFunc != NULL)
        pNxDevice->SetXShutPinLevelFunc(level);
#else
    SetXshutPinLevelLib(level);
#endif
} 

/**
 * @brief Write 1 Byte to ND03
 *        对ND03的寄存器写1个字节
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param wdata: 寄存器的值
 * @return int32_t
*/
int32_t ND03_WriteByte(ND03_Dev_t *pNxDevice, uint16_t addr, uint8_t wdata)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret = I2C_WriteNBytesLib(pNxDevice->i2c_dev_addr, addr, &wdata, 1);

	return ret;
}


/**
 * @brief Write 4 Bytes to ND03
 *        对ND03的寄存器写1个字
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param wdata: 寄存器的值
 * @return int32_t
*/
int32_t ND03_WriteWord(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t wdata)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint8_t     buf[4];

    buf[0] = (wdata >> 0) & 0xff;
    buf[1] = (wdata >> 8) & 0xff;
    buf[2] = (wdata >>16) & 0xff;
    buf[3] = (wdata >>24) & 0xff;
    ret = I2C_WriteNBytesLib(pNxDevice->i2c_dev_addr, addr, buf, 4);

	return ret;
}


/**
 * @brief Read 4 Bytes from ND03
 *        对ND03的寄存器读1个字
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param rdata: 存放寄存器值的指针
 * @return int32_t
*/
int32_t ND03_ReadWord(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata)
{
    int32_t     ret = ND03_ERROR_NONE;
    uint8_t     buf[4];

    ret = I2C_ReadNBytesLib(pNxDevice->i2c_dev_addr, addr, buf, 4);
    *rdata = ((uint32_t)buf[3]<<24) | ((uint32_t)buf[2]<<16) | ((uint32_t)buf[1]<<8) | (uint32_t)buf[0];

	return ret;
}


/**
 * @brief Write n Words to ND03
 *        对ND03的寄存器写N个字
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param wdata: 存放寄存器值的指针
 * @param len: 写数据的长度，按字个数计算
 * @return int32_t
*/
int32_t ND03_WriteNWords(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *wdata, uint32_t len)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret |= I2C_WriteNBytesLib(pNxDevice->i2c_dev_addr, addr, (uint8_t*)wdata, len*4);

	return ret;
}


/**
 * @brief Read n Words from ND03
 *        对ND03的寄存器读N个字
 * @param pNxDevice: ND03模组设备信息结构体指针
 * @param addr: 寄存器地址
 * @param rdata: 存放寄存器值的指针
 * @param len: 读数据的长度，按字个数计算
 * @return int32_t
*/
int32_t ND03_ReadNWords(ND03_Dev_t *pNxDevice, uint16_t addr, uint32_t *rdata, uint32_t len)
{
    int32_t     ret = ND03_ERROR_NONE;

    ret |= I2C_ReadNBytesLib(pNxDevice->i2c_dev_addr, addr, (uint8_t*)rdata, len*4);

	return ret;
}

