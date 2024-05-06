/**
 * @file nd03_def.h
 * @author tongsheng.tang
 * @brief ND03's Macro definition and data structure
 * @version 1.x.x
 * @date 2021-11
 * 
 * @copyright Copyright (c) 2021, Shenzhen Nephotonics Inc.
 * 
 */
#ifndef __ND03__DEF__H__
#define __ND03__DEF__H__

#include "nd03_stdint.h"
#if ND03_PLATFORM == PLATFORM_LINUX_DRIVER
#include <linux/kernel.h>
#else
#include <stdio.h>
#endif



#define DEBUG_INFO        0 /** 调试信息打印开关 */

#if ND03_PLATFORM == PLATFORM_NOT_C51
#define NX_PRINTF(fmt, ...)   do{if(DEBUG_INFO) printf(fmt, ##__VA_ARGS__); }while(0) /** 调试接口，默认使用printf */
#elif ND03_PLATFORM == PLATFORM_LINUX_DRIVER
#define NX_PRINTF       if(DEBUG_INFO) printk  /** 调试接口，默认使用printf */
#elif DEBUG_INFO
#define NX_PRINTF       printf
#else
#define NX_PRINTF       /##/
#endif

/** @defgroup ND03_Global_Define_Group ND03 Defines
 *	@brief	  ND03 Defines
 *	@{
 */


/** @defgroup ND03_Fw_Ver_Value_Group ND03 Version Value Defines
 * @brief	  ND03 Firmware Version Value
 *  @{
 */

#define ND03_FW_VERSION_V102_VALUE              (10002)   /** V1.0.2版本数值 */
#define ND03_FW_VERSION_V103_VALUE              (10003)   /** V1.0.3版本数值 */
#define ND03_FW_VERSION_V104_VALUE              (10004)   /** V1.0.4版本数值 */
#define ND03_FW_VERSION_V200_VALUE              (20000)   /** V2.0.0版本数值 */

/** @} ND03_Fw_Ver_Value_Group */


/** @defgroup ND03_Reg_Group ND03 Register Defines
 * @brief	  ND03 Register Defines
 *  @{
 */

#define ND03_REG_BOOT1                          (0x55AA)  /** ND03模组第一段启动寄存器，需要对该寄存器写1byte，且值为0x38 */
#define ND03_REG_BOOT2                          (0x3BFC)  /** ND03模组第二段启动寄存器，需要对该寄存器写4bytes，且值为0xFFFFFFF2 */
#define ND03_REG_BOOT3                          (0x0888)  /** ND03模组第三段启动寄存器，需要对该寄存器写4bytes，且值为0x00000001 */
#define ND03_REG_BOOT4                          (0x3A1C)  /** ND03模组第四段启动寄存器，需要对该寄存器写4bytes，且值为0x00000030 */
#define ND03_REG_BOOT5                          (0x0808)  /** ND03模组第五段启动寄存器，需要对该寄存器写4bytes，且值为0x00000001 */
#define ND03_REG_DEV_ID                         (0x3B80)  /** ND03模组设备ID */

#define ND03_REG_CALIB_ENA1                     (0xA770)  /** 标定使能1 */
#define ND03_REG_CALIB_ENA2                     (0xA774)  /** 标定使能2 */
#define ND03_REG_SAMPLE_NUM                     (0xA778)  /** 数据采样次数寄存器，环境光越大，该值设置越大越好，但最好不要超过10，以及越大会增加测量时间 */
#define ND03_REG_THERM_COE_NORMAL               (0xA780)  /** 温度系数寄存器 */
#define ND03_REG_RANGING_STATUS                 (0xA784)  /** 测距状态寄存器 */
#define ND03_REG_THERM_COE_FAST                 (0xA788)  /** 快速模式温度系数寄存器 */
#define ND03_REG_CLK_CNT                        (0xA788)  /** 时钟个数寄存器，单位：KHz */
#define ND03_REG_MODEL                          (0xA78C)  /** 模组型号寄存器 */
#define ND03_REG_TARGET_AMP                     (0xA790)  /** 目标幅度值寄存器 */
#define ND03_REG_TARGET_AMP_ERROR               (0xA794)  /** 目标幅度容错值寄存器 */

/* Read Only(RO) */
#define ND03_REG_FW_VERSION                     (0xA7A0)  /** ND03模组固件版本 */
#define ND03_REG_STATE                          (0xA7A4)  /** ND03模组运行状态 */
#define ND03_REG_AMP_DEPTH                      (0xA7A8)  /** 幅度值和深度值寄存器，[15:0]->uint16_t:深度值(mm), [31:16]->uint16_t:幅度值 */
#define ND03_REG_EXP_THERM                      (0xA7AC)  /** 积分时间和温度值寄存器, [15:0]->int16_t:温度值(0.1), [31:16]->uint16_t:积分时间(us)*/
#define ND03_REG_ERROR_FLAG                     (0xA7B0)  /** 错误返回寄存器 */
#define ND03_REG_XTALK_AMP                      (0xA7B4)  /** 串扰幅度值寄存器 */
#define ND03_REG_XTALK_DEPTH                    (0xA7B8)  /** 串扰深度值寄存器 */


/* Read and Write(RW) */
#define ND03_REG_CALIB_XT_OFFSET                (0xA7BC)  /** 偏移量标定和串扰标定的实际深度寄存器， [15:0]->uint16_t:标定深度(mm), [31:16]->uint16_t:串扰标定深度(mm)*/
#define ND03_REG_RANGE_MODE                     (0xA7C0)  /** 测距模式配置寄存器 */
#define ND03_REG_SLAVE_ADDR                     (0xA7C4)  /** 模组IIC设备地址寄存器 */
#define ND03_REG_DATA_VAL_REQ                   (0xA7C8)  /** 数据请求和数据有效寄存器, [15:0]->uint16_t:数据请求, [31:16]->uint16_t:数据有效*/
#define ND03_REG_TOF_FLAG                       (0xA7CC)  /** TOF Flag寄存器 */
#define ND03_REG_MAX_EXPOSURE                   (0xA7D0)  /** 最大积分时间寄存器 */
#define ND03_REG_FLASH_ADDR                     (0xA7D4)  /** 读取flash数据的地址寄存器 */
#define ND03_REG_FLASH_DATA                     (0xA7D8)  /** 读取flash的数据寄存器 */
#define ND03_REG_GPIO_DEPTH_TH                  (0xA7DC)  /** GPIO功能远近距离阈值寄存器, [15:0]->uint16_t:近距离阈值, [31:16]->uint16_t:远距离阈值 */
#define ND03_REG_GPIO_SETTING                   (0xA7E0)  /** GPIO中断输出功能配置寄存器 */
#define ND03_REG_SETTING                        (0xA7E4)  /** 其他功能配置寄存器 */
#define ND03_REG_MOD_FREQ                       (0xA7E8)  /** 调制频率寄存器 */

#define ND03_REG_V102_DEPTH                     (0x8298)  /** V1.0.2版本的深度值寄存器 */
#define ND03_REG_V102_AMP                       (0x82A0)  /** V1.0.2版本的幅度值寄存器 */
#define ND03_REG_V103_DEPTH                     (0x828C)  /** V1.0.3版本的深度值寄存器 */
#define ND03_REG_V103_AMP                       (0x8294)  /** V1.0.3版本的幅度值寄存器 */

#define ND03_REG_V102_XTALK_AMP                 (0x8344)  /** V1.0.2版本的串扰幅度值寄存器 */
#define ND03_REG_V102_XTALK_DEPTH               (0x8354)  /** V1.0.2版本的串扰深度值寄存器 */
#define ND03_REG_V103_XTALK_AMP                 (0x8338)  /** V1.0.3版本的串扰幅度值寄存器 */
#define ND03_REG_V103_XTALK_DEPTH               (0x8348)  /** V1.0.3版本的串扰深度值寄存器 */
#define ND03_REG_V104_XTALK_AMP                 (0x8354)  /** V1.0.4版本的串扰幅度值寄存器 */
#define ND03_REG_V104_XTALK_DEPTH               (0x8364)  /** V1.0.4版本的串扰深度值寄存器 */


#define ND03_REG_V102_CALIB_ERROR               (0x82C8)  /** V1.0.2版本数值 */
#define ND03_REG_V103_CALIB_ERROR               (0x82B4)  /** V1.0.3版本数值 */
#define ND03_REG_V104_CALIB_ERROR               (0x82B8)  /** V1.0.4版本数值 */

#define ND03_REG_V103_TARGET_AMP                (0x82EC)  /** V1.0.3目标幅度值寄存器 */
#define ND03_REG_V103_TARGET_AMP_ERROR          (0x82F0)  /** V1.0.3目标幅度容错值寄存器 */

#define ND03_REG_SLEEP_SOFTWARE                 (0x3BFC)  /** 软件睡眠寄存器 */

/** @} ND03_Reg_Group */

/** @defgroup ND03_Depth_Value_Group ND03 Depth Value Defines
 * @brief	  ND03 Depth Value
 *  @{
 */

#define ND03_DEPTH_OVERFLOW                     (65500)   /** 过曝时深度值 */
#define ND03_DEPTH_FILTER                       (65400)   /** 跨周期过滤深度值 */
#define ND03_DEPTH_LOW_AMP                      (65300)   /** 欠曝时深度值 */

/** @} ND03_Depth_Value_Group */

/** @defgroup ND03_Boot_Group ND03 Boot Value
 *	@brief	  ND03 Boot  (ND03_REG_BOOT)
 *	@{
 */

#define ND03_REG_BOOT1_VALUE                      0x38          /** ND03模组第一段启动值，且值为0x38 */
#define ND03_REG_BOOT2_VALUE                      0xFFFFFFF2    /** ND03模组第二段启动值，且值为0xFFFFFFF2 */
#define ND03_REG_BOOT3_VALUE                      0x00000001    /** ND03模组第三段启动值，且值为0x00000001 */
#define ND03_REG_BOOT4_VALUE                      0x00000030    /** ND03模组第四段启动值，且值为0x00000030 */
#define ND03_REG_BOOT5_VALUE                      0x00000001    /** ND03模组第五段启动值，且值为0x00000001 */
#define ND03_REG_DEV_ID_VALUE                     0x002F0012    /** ND03模组设备ID默认值 */

/** @} ND03_Boot_Group */


/** @defgroup ND03_State_Group ND03 Data Request Index
 *	@brief	  ND03 State  (ND03_REG_STATE)
 *	@{
 */

#define ND03_STATE_IDLE                           0x00000000
#define ND03_STATE_SOFTWARE_READY                 0x000000A5
#define ND03_STATE_GOT_DEPTH                      0x000000A6

/** @} ND03_State_Group */

/** @defgroup ND03_Data_Val_Req_Idx_Group ND03 Data Request Index
 *	@brief	  ND03 Data Request Mask (ND03_REG_DATA_VAL_REQ)
 *	@{
 */
// REG_DATA_REQ MASK
#define ND03_DEPTH_DATA_REQ_MASK                  0x00000001  /** 深度数据读取请求掩码 */
#define ND03_OFFSET_CALIBRATION_REQ_MASK          0x00000002  /** 偏移量标定请求掩码 */
#define ND03_XTALK_CALIB_REQ_MASK                 0x00000004  /** 串扰标定请求掩码 */
#define ND03_FLASH_DATA_REQ_MASK                  0x00000100  /** flash数据读取请求掩码 */
// REG_DATA_VAL MASK
#define ND03_DEPTH_DATA_VAL_MASK                  0x00010000  /** 深度数据有效位掩码 */
#define ND03_OFFSET_CALIBRATION_VAL_MASK          0x00020000  /** 偏移量标定完成有效位掩码 */
#define ND03_XTALK_CALIB_VAL_MASK                 0x00040000  /** 串扰标定完成有效位掩码 */
#define ND03_FLASH_DATA_VAL_MASK                  0x01000000  /** flash数据有效位掩码 */

/** @} ND03_Data_Val_Req_Idx_Group */



/** @defgroup ND03_Tof_Flag_Mask_Group ND03 Setting Mask 
 *	@brief	  ND03 Tof Flag Mask (ND03_REG_TOF_FLAG)
 *	@{
 */

// REG_TOF_FLAG Mask
#define ND03_THERM_TOF_FLAG                       0x00000001  /** 温度补偿配置掩码 */
#define ND03_OFFSET_TOF_FLAG                      0x00000100  /** 偏移量矫正配置掩码 */
#define ND03_XT_TOF_FLAG                          0x00010000  /** 串扰矫正配置掩码 */
#define ND03_AE_TOF_FLAG                          0x01000000  /** 自动曝光配置掩码 */

/** @} ND03_Tof_Flag_Mask_Group */



/** @defgroup ND03_Gpio_Func_Group ND03 Gpio Functions Define
 *	@brief	  ND03 Gpio Functions Define Group (ND03_REG_GPIO_SETTING)
 *	@{
 */

/** REG_INT_SETTING Mask */
#define ND03_GPIO_FUNCTIONALITY_MASK              0x00000007  /** GPIO功能配置掩码 */
#define ND03_GPIO_POLARITY_MASK                   0x00000008  /** 极性配置掩码 */
#define ND03_GPIO_RESTART_MEASURE_MASK            0x00000010  /** 内部自动计算掩码 */


/** GPIO Functionality */
typedef uint32_t  ND03_GpioFunc_t;
#define ND03_GPIO_FUNCTIONALITY_OFF               ((ND03_GpioFunc_t) 0x00000000) /** 无触发中断 */
#define ND03_GPIO_THRESHOLD_LOW                   ((ND03_GpioFunc_t) 0x00000001) /** 低深度触发中断 (value < REG_GPIO_DEPTH_LOW_TH) */
#define ND03_GPIO_THRESHOLD_HIGH                  ((ND03_GpioFunc_t) 0x00000002) /** 高深度触发中断 (value > REG_GPIO_DEPTH_HIGH_TH) */
#define ND03_GPIO_THRESHOLD_DOMAIN_OUT            ((ND03_GpioFunc_t) 0x00000003) /** 低深度或高深度触发中断 (value < REG_GPIO_DEPTH_LOW_TH 或 value > REG_GPIO_DEPTH_HIGH_TH) */
#define ND03_GPIO_NEW_MEASURE_READY               ((ND03_GpioFunc_t) 0x00000004) /** 新深度数据就绪中断 */

/** GPIO polarity */
typedef uint32_t ND03_GpioPolar_t;
#define ND03_GPIO_POLARITY_LOW                    ((ND03_GpioPolar_t) 0x00000000) /** 负极性, 低电平有效 */
#define ND03_GPIO_POLARITY_HIGH                   ((ND03_GpioPolar_t) 0x00000001) /** 正极性, 高电平有效 */

/** @} ND03_Gpio_Func_Group */


/** @defgroup ND03_Setting_Group ND03 Setting Group
 *  @brief	  ND03 Setting Group (ND03_REG_SETTING)
 *	@{
 */
#define ND03_SETTING_SW_SLEEP_MASK                0x00000001
/** @} ND03_Setting_Group */


/** @defgroup ND03_Sleep_Group ND03 Sleep Group
 *  @brief	  ND03 Sleep Group (ND03_REG_SLEEP_SOFTWARE)
 *	@{
 */
#define ND03_SLEEP_SOFTWARE_VALUE                 0xFFFFFFF1
/** @} ND03_Sleepg_Group */


/** @defgroup ND03_Error_Group ND03 Error Group
 *  @brief	  ND03 Error Group (ND03_REG_ERROR_FLAG)
 *	@{
 */
#define ND03_ERROR_NONE                            0  /** 成功 */
#define ND03_ERROR_CALIBRATION                    -1  /** 标定失败错误 */
#define ND03_ERROR_TIME_OUT                       -2  /** 超时错误 */
#define ND03_ERROR_I2C			                      -3  /** IIC通讯错误 */
#define ND03_ERROR_AMP			                      -4  /** 幅度值错误 */
#define ND03_ERROR_FW                             -5  /** 固件版本兼容错误 */
#define ND03_ERROR_BOOT                           -6  /** 模组启动错误 */
#define ND03_ERROR_WORD_SIZE                      -7  /** 字的大小配置错误 */
#define ND03_ERROR_RANGING                        -8  /** 测距出错 */

#define ND03_ERROR_XTALK_AMP_OVERFLOW             33  /** 串扰幅度值过大 */
#define ND03_ERROR_XTALK_CALIBRATION              34  /** 串扰标定失败 */
/** @} ND03_Error_Group */


/** @defgroup ND03_Device_Mode_Group ND03 Device Mode
 *  @brief  ND03 Device Mode (ND03_REG_RANGE_MODE)
 *	@{
 */

#define ND03_RANGE_MODE_MASK                      ((uint32_t)0x00000001)              /** 测量模式 */
#define ND03_DEVICEMODE_MASK                      ((uint32_t)0x00000002)              /** 设备机制模式 */
#define ND03_CALCU_MODE_MASK                      ((uint32_t)0x00000008)              /** 计算模式 */

#define ND03_RANGE_MODE_NORMAL                    ((uint32_t)0x00000000)              /** 普通测量模式 */
#define ND03_RANGE_MODE_FAST                      ((uint32_t)0x00000001)              /** 快速测量模式 */

#define ND03_DEVICEMODE_SINGLE_RANGING            ((uint32_t)0x00000000)              /** 单次测量模式 */
#define ND03_DEVICEMODE_CONTINUOUS_RANGING        ((uint32_t)0x00000002)              /** 连续测量模式 */

#define ND03_CALCU_MODE_SPOT                      ((uint32_t)0x00000000)              /** 单点计算模式 */
#define ND03_CALCU_MODE_AVER                      ((uint32_t)0x00000008)              /** 平均计算模式 */

/** @} ND03_Device_Mode_Group */



/** @defgroup ND03_Slave_Address_Group ND03 Device Mode
 *  @brief  ND03 Slave Address (ND03_REG_Slave_ADDR)
 *	@{
 */

#define ND03_DEFAULT_SLAVE_ADDR                   0x5B  /** ND03默认IIC地址(7bit) */

/** @} ND03_Global_Define_Group */



/** @} ND03_Global_Define_Group */

/** @enum  ND03_Status_e
 *  @brief 定义ND03状态宏
 */
typedef enum{
    ND03_DISABLE = 0,   ///< 关闭状态
    ND03_ENABLE  = 1    ///< 使能状态
  } ND03_Status_e;

/** @enum  ND03_GPIOPolarity_e
 *  @brief 定义模组GPIO输出极性
 */
typedef enum{
    ND03_GPIO_POLAR_LOW   = 0,    ///< 低电平有效
    ND03_GPIO_POLAR_HIGH  = 1,    ///< 高电平有效
}ND03_GPIOPolarity_e;


/** 
  * @struct ND03_RangingData_t
  * 
  * @brief ND03测量结果结构体 \n
  * 定义存储ND03的深度、幅度信息
  */
typedef struct{
    uint16_t depth; ///< 测量距离/mm
    uint16_t amp;   ///< 测量幅度值，作为信号可信度
}ND03_RangingData_t;


/** 
  * @struct ND03_ChipInfo_t
  * 
  * @brief ND03模组生产信息\n
  */
 typedef struct {
    uint32_t  nd03_fw_version;        ///< ND03固件版本
    uint32_t  nd03_product_date;      ///< ND03生产日期
    uint32_t  nd03_product_time;      ///< ND03生产时间
    uint32_t  nd03_model_revision;    ///< ND03设备类型及硬件版本
    uint8_t   nd03_chip_id[32];       ///< ND03唯一芯片ID,字符串
 } ND03_ChipInfo_t;

/** 
  * @struct ND03_DevConfig_t
  * 
  * @brief ND03模组配置数据\n
  */
typedef struct {
    uint32_t        range_mode;             ///< 模组取图模式配置
} ND03_DevConfig_t;

/** 
  * @struct ND03_Dev_t
  * 
  * @brief 设备类型结构体\n
  */
typedef struct {
    void(*SetXShutPinLevelFunc)(uint32_t );   ///< 如果设置XShut电平函数指针，参数值为0，则输出低电平，否则输出高电平
    ND03_DevConfig_t  config;                 ///< 模组配置信息
    ND03_ChipInfo_t   chip_info;              ///< 模组生产信息
    uint8_t           i2c_dev_addr;           ///< i2c设备地址
    uint32_t          dev_pwr_state;          ///< 设备的当前状态, 就绪模式或者休眠模式
} ND03_Dev_t;


#endif

