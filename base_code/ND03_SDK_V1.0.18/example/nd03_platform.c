/*
    This file list all the functions for user to implement.
*/

#include <stdio.h>


#include "nd03_platform.h"

//#define I2C_24C02_DEV_ADDR 0x50   //i2c设备地址

//#define I2C_TP_GT911_DEV_ADDR 0x28   //i2c设备地址,0x28,29

static int i2c_fd = 0;


void delay_1ms(uint32_t count)
{
    //For user implement
    usleep(1000);
}

void delay_10us(uint32_t count)
{
    //For user implement
    usleep(10);
}

void set_xshut_pin_level(uint32_t level)
{
    //For user implement
    return 0;
}


/**
 * i2c初始化函数
 *
 * */
static int i2c_init(void)
{
	int fd = 0, ret = 0;
	if ((fd = open("/dev/i2c-0", O_RDWR)) < 0)
	{
		printf("open %s failed:%s\n", "/dev/i2c-0", strerror(errno));
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
    printf("i2c init successful \n");
	return 0;
}
/**
 * 
 * */
static int i2c_exit(void)
{
    if (i2c_fd > 0)
		close(i2c_fd);
	return 0;
}

/**
 * ND03A 寄存器地址为16bit，这里吧reg的char类型改成了int
 * */
static int i2c_read_bytes_dou_reg(int fd,unsigned char dev_addr,int reg_addr,unsigned char *buf,unsigned int len)
{
	int ret = 0;
    struct i2c_msg message[2];
    int tmp1, tmp2;
    tmp1 = ((reg_addr & 0xff00)>>8); // 0x1234 & 0xff00 = 0x1200 >> 8 = 0x0012
    tmp2 = ((reg_addr & 0x00ff)<<8); // 0x1234 & 0x00ff = 0x0034 << 8 = 0x3400
    reg_addr = tmp2 | tmp1;

    message[0].addr = dev_addr;//设备地址
    message[0].flags = 0;   //写标志
    message[0].buf = &reg_addr;//寄存器地址
    message[0].len = 2;//sizeof(reg_addr); //这里只取前16bit，两个字节

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
 * 这里由于ND03A的寄存器地址是多双字节的，因此，这里需要有两个寄存器地址
 * */
static int i2c_write_bytes_dou_reg(int fd,unsigned char dev_addr,int reg_addr, unsigned char *p_value_bytes,unsigned int len)
{
	int ret = 0,i = 0;
    unsigned char buf[10]={0};
    unsigned char reg_addr1, reg_addr2;
    struct i2c_msg message;
    
    reg_addr1 = (reg_addr & 0xFF00)>>8;
    reg_addr2 = (reg_addr & 0x00FF);
    //printf("\nreg_addr1: 0x%x, reg_addr2: 0x%x\n", reg_addr1, reg_addr2);

    buf[0] = reg_addr1;//寄存器地址1
    buf[1] = reg_addr2;//寄存器地址2

    for(i = 0;i < len;i++){
        buf[i+2] = p_value_bytes[i];
        //printf("buf[%d]= 0x%x \n", i+2, buf[i+2]);
    }
    
    message.addr = dev_addr;//设备地址
    message.buf = buf;
    message.flags = 0;//写标志
    message.len = len+2;//data+reg_addr

    i2c_data.msgs = &message;
    i2c_data.nmsgs = 1;

	ret = ioctl(fd, I2C_RDWR, &i2c_data);
	if (ret < 0)
	{
		printf("%s:%d write data error:%s\n", __func__, __LINE__, strerror(errno));
		return -1;
	}
	return 0;
}

int32_t i2c_read_nbytes(uint8_t i2c_addr, uint16_t i2c_read_addr, uint8_t *i2c_read_data, uint8_t len)
{
    //For user implement
    int ret = -1;

    if (0 == i2c_fd)
    {
        ret = i2c_init();
        if (ret < 0)
        {
            return -1;
        }
    }
    i2c_read_bytes_dou_reg(i2c_fd, i2c_addr, i2c_read_addr, i2c_read_data, len);

    return 0;
}

int32_t i2c_write_nbytes(uint8_t i2c_addr, uint16_t i2c_write_addr, uint8_t *i2c_write_data, uint8_t len)
{
    //For user implement
    int ret = -1;

    if (0 == i2c_fd)
    {
        ret = i2c_init();
        if (ret < 0)
        {
            return -1;
        }
    }
    i2c_write_bytes_dou_reg(i2c_fd, i2c_addr, i2c_write_addr, i2c_write_data, len);

    return 0;
}