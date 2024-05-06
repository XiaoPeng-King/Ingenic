/*
    This file list all the functions for user to implement.
*/

#include <stdio.h>
#include <linux/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "nd03_platform.h"

//#define I2C_24C02_DEV_ADDR 0x50   //i2c设备地址

#define I2C_TP_GT911_DEV_ADDR 0x28   //i2c设备地址,0x28,29


const char default_i2c[] = "/dev/i2c-2";
char *path = 0;

static int i2c_fd;
struct i2c_rdwr_ioctl_data i2c_data;

int32_t i2c_read_nbytes(uint8_t i2c_addr, uint16_t i2c_read_addr, uint8_t *i2c_read_data, uint8_t len)
{
    //For user implement


    i2c_read_bytes();

    return 0;
}

int32_t i2c_write_nbytes(uint8_t i2c_addr, uint16_t i2c_write_addr, uint8_t *i2c_write_data, uint8_t len)
{
    //For user implement

    return 0;
}


void delay_1ms(uint32_t count)
{
    //For user implement
}

void delay_10us(uint32_t count)
{
    //For user implement
}


void set_xshut_pin_level(uint32_t level)
{
    //For user implement
}


/**
 * i2c初始化函数
 *
 * */
int i2c_init(void)
{
	int fd = 0, ret = 0;
	if ((fd = open(path, O_RDWR)) < 0)
	{
		printf("open %s failed:%s\n", path, strerror(errno));
		return -1;
    }
    if ((ret = ioctl(fd, I2C_TIMEOUT, 100)) < 0) //超时时间
    {
        printf("set i2c timeout failed:%s\n", strerror(errno));
        close(fd);
        return -1;
    }
    if ((ret = ioctl(fd, I2C_RETRIES, 5)) < 0) //重复次数
    {
        printf("set i2c retries failed:%s\n", strerror(errno));
        close(fd);
        return -1;
    }
    if ((ret = ioctl(fd, I2C_TENBIT, 0)) < 0) //地址模式 7
    {
        printf("set i2c Address mode failed:%s\n", strerror(errno));
        close(fd);
        return -1;
    }
    i2c_fd=fd;
	return 0;
}
/**
 * 
 * */
int i2c_exit(void)
{
    if (i2c_fd > 0)
		close(i2c_fd);
	return 0;
}


/**
 * i2c 单字节读取
 * */
int i2c_read_byte(int fd,unsigned char dev_addr,unsigned char reg_addr,unsigned char *buf)
{
	int ret = 0;
	int dev_node = i2c_fd;
    struct i2c_msg message[2];

    message[0].addr = dev_addr;//设备地址
    message[0].flags = 0;   //写标志
    message[0].buf = &reg_addr;//寄存器地址
    message[0].len = sizeof(reg_addr);

    message[1].addr = dev_addr;//设备地址
    message[1].flags = I2C_M_RD; //读标志
    message[1].buf = buf;
    message[1].len = sizeof(reg_addr);

    i2c_data.msgs = message;
    i2c_data.nmsgs = 2;

    ret = ioctl(dev_node, I2C_RDWR, (unsigned long)&i2c_data);
    if (ret < 0)
    {
		printf("%s:%d I2C: read error:%s\n", __func__, __LINE__, strerror(errno));
		return ret;
	}
	return 0;
}

/**
 * i2c 多字节读取
 * */
int i2c_read_bytes(int fd,unsigned char dev_addr,unsigned char reg_addr,unsigned char *buf,unsigned int len)
{
	int ret = 0;
    struct i2c_msg message[2];

    message[0].addr = dev_addr;//设备地址
    message[0].flags = 0;   //写标志
    message[0].buf = &reg_addr;//寄存器地址
    message[0].len = sizeof(reg_addr);

    message[1].addr = dev_addr;//设备地址
    message[1].flags = I2C_M_RD; //读标志
    message[1].buf = buf;
    message[1].len = sizeof(unsigned char)*len;

    i2c_data.msgs = message;
    i2c_data.nmsgs = 2;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if (ret < 0)
    {
		printf("%s:%d I2C: read error:%s\n", __func__, __LINE__, strerror(errno));
		return ret;
	}
	return 0;
}



/**
 * i2c 单字节写
 * */
int i2c_write_byte(int fd,unsigned char dev_addr,unsigned char reg_addr,unsigned char value_byte)
{
	int ret = 0;
    unsigned char buf[2]={0};
    struct i2c_msg message;
    
    buf[0] = reg_addr;//寄存器地址
    buf[1] = value_byte;//写入的字节
   
    message.addr = dev_addr;//设备地址
    message.buf = buf;
    message.flags = 0;//写标志
    message.len = 2;

    i2c_data.msgs = &message;
    i2c_data.nmsgs = 1;

	ret = ioctl(fd, I2C_RDWR,&i2c_data);
	if (ret < 0)
	{
		printf("%s:%d write data error:%s\n", __func__, __LINE__, strerror(errno));
		return -1;
	}
	return 0;
}

/**
 * i2c 多字节写
 * */
int i2c_write_bytes(int fd,unsigned char dev_addr,unsigned char reg_addr,unsigned char *p_value_bytes,unsigned int len)
{
	int ret = 0,i = 0;
    unsigned char buf[10]={0};
    struct i2c_msg message;
    
    buf[0] = reg_addr;//寄存器地址
    for(i = 0;i < len;i++){
        buf[i+1] = p_value_bytes[i];
    }
    message.addr = dev_addr;//设备地址
    message.buf = buf;
    message.flags = 0;//写标志
    message.len = len+1;//data+reg_addr

    i2c_data.msgs = &message;
    i2c_data.nmsgs = 1;

	ret = ioctl(fd, I2C_RDWR,&i2c_data);
	if (ret < 0)
	{
		printf("%s:%d write data error:%s\n", __func__, __LINE__, strerror(errno));
		return -1;
	}
	return 0;
}