/**********************************************************
 * Author        : kaiwang
 * Email         : kai.wang@ingenic.com
 * Last modified : 2020-09-17 19:55
 * Filename      : sample_i2c.c
 * Description   : 
 * *******************************************************/
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

//#define I2C_24C02_DEV_ADDR 0x50   //i2c设备地址

#define I2C_DW9714_DEV_ADDR 0x0C //0x18   


const char default_i2c[] = "/dev/i2c-1";
char *path = 0;

static int i2c_fd;
struct i2c_rdwr_ioctl_data i2c_data;
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


#if 0
/**
 * i2c 单字节读取
 * */
int i2c_read_byte(int fd,unsigned char dev_addr,unsigned char reg_addr,unsigned char *buf)
{
	int ret = 0;
	int dev_node = i2c_fd; //文件句柄
    struct i2c_msg message[2]; //消息个数

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
#endif

/**
 * 
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
 * DW9714 i2c 读取操作
 * 由于 DW9714 读取的时候没有寄存器地址，因此，我们直接写设备地址，然后就开始读取数据
 * 
 * 
 * */
int i2c_read_byte_DW9712(int fd,unsigned char dev_addr, unsigned char *buf, unsigned int len)
{
	int ret = 0;
    struct i2c_msg message[2];

    message[0].addr = dev_addr;//设备地址
    message[0].flags = 0;   //写标志
    //message[0].buf = &reg_addr;//寄存器地址
    message[0].len = 0; //sizeof(reg_addr);

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


#if 0
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
#endif

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

/**
 * i2c DW9714
 * 这里DW9714 写操作只需要设备地址就可以了，没有寄存器地址，因此，这里的buf里面直接写数据，没有放寄存器地址进去。
 * 写操作格式如下：设备地址，数据1，数据2
 * 这里的设备地址是 0x18, 然后DW9714的默认只有两个8bit数据可以操作。
 * */
int i2c_write_byte_DW9714(int fd,unsigned char dev_addr,unsigned char *p_value_bytes, unsigned int len)
{
	int ret = 0,i = 0;
    unsigned char buf[10]={0};
    struct i2c_msg message;

    for(i = 0;i < len;i++){
        buf[i] = p_value_bytes[i];
    }
   
    message.addr = dev_addr;//设备地址
    message.buf = buf;
    message.flags = 0;//写标志
    message.len = len;

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
 * I2C功能使用，支持标准Linux接口
 * sample_i2c.c目的是测试I2C功能,如果使用请按照具体开发需要更改
 * 这里以24c02读写为例测试I2C功能
 * */
int main(int argc, char *argv[])
{
	int ret = 0;
    int i = 0;
    unsigned char read_byte = 0;

    //0011 1111 1111 1000
    unsigned char write_data_arr0[10] = {0x3F,0xF8,0x12,0x13,0x14,0x15,0x16,0x17};
    //0000 0000 1111 1000
    unsigned char write_data_arr1[10] = {0x00,0xF8,0x12,0x13,0x14,0x15,0x16,0x17};
    unsigned char read_data_arr[10] = {0};
    if(argc > 1){
        path = argv[1];
    }else{
        path = (char *)default_i2c;
    }

    ret = i2c_init();
    if(ret < 0){
        printf("i2c_init failed\n");
        return 0;
    }


    while (1) {

        i2c_write_byte_DW9714(i2c_fd, I2C_DW9714_DEV_ADDR, write_data_arr0, 2);
        usleep(1000000);
        i2c_write_byte_DW9714(i2c_fd, I2C_DW9714_DEV_ADDR, write_data_arr1, 2);
        usleep(1000000);

        //i2c_read_byte_DW9712(i2c_fd, I2C_DW9714_DEV_ADDR, read_data_arr, 2);
        //usleep(500000);
    }

    i2c_exit();
    return ret;
}
