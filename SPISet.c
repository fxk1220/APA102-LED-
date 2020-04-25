/*************************************************************************
   Copyright: (C) 2019 xinkui.fu  
   All rights reserved. 
   File Name: SPISet.c
   Description: 
   
   Author: 
   Mail: 
   Date: Mon 18 Feb 2019 06:49:48 UTC
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include "SPISet.h"

static const char *spiDev0 = "/dev/spidev0.0";
static const char *spiDev1 = "/dev/spidev0.1";

static uint8_t spiBPW = 8;
static uint16_t spiDelay = 0;

static uint32_t spiSpeeds[2];
static int spiFds[2];

/*
 * SPIDataRW:
 *  Write and Read a block of data over the SPI bus 
 *  Note the data is being read into the transmit buffer,so will 
 *  overwrite it ;
 *  This is also a full-duplex operation .
 *  
 */

int SPIDataRW(int channel, unsigned char *tx_data,unsigned char *rx_data, int len)
{
    int i = 0; 
    struct spi_ioc_transfer spi;
    channel &= 1;
    memset(&spi, 0, sizeof(spi));
    spi.tx_buf = (unsigned char *)tx_data;
    spi.rx_buf = (unsigned char *)rx_data;
    spi.len = len;
    spi.delay_usecs = spiDelay;
    spi.speed_hz = spiSpeeds[channel];
    spi.bits_per_word = spiBPW;
    return ioctl(spiFds[channel],SPI_IOC_MESSAGE(1),&spi);//SPI_IOC_MESSAGE(1)的1表示spi_ioc_transfer的数量
}

/*
 * SPISetupMode:
 *   Open the SPI device , and set it up, with the mode ,etc
 */

int SPISetupMode(int channel, int speed, int mode)
{
    int fd; 
    if((fd = open(channel == 0 ? spiDev0:spiDev1, O_RDWR)) < 0){
        printf("Unable to open SPI device :%s \r\n",strerror(errno));
        return -1;
    }
    spiSpeeds[channel] = speed;
    spiFds[channel] = fd;

/*
 *  设置 spi的读写模式
 *  Mode 0:  CPOL = 0, CPHA = 0;
 *  Mode 1:  CPOL = 0, CPHA = 1;
 *  Mode 2:  CPOL = 1, CPHA = 0;
 *  Mode 3:  CPOL = 1, CPHA = 1;
 *  Set default mode (Mode 0);
 */
    if(ioctl(fd, SPI_IOC_WR_MODE, &mode)< 0){
        printf("Can't set spi mode:%s \r\n",strerror(errno));
        return -1;
    }
    
    if(ioctl(fd, SPI_IOC_RD_MODE,&mode) < 0){
        printf("Can't get spi mode:%s \r\n",strerror(errno));
        return -1;
    }
/*
 * spi的读写bit/word 设置可写
 *   这里设为8位为一个字节
 */
    if(ioctl(fd, SPI_IOC_WR_BITS_PER_WORD,&spiBPW)< 0){
        printf("Can't set bits per word:%s \r\n",strerror(errno));
        return -1;
    }

    if(ioctl(fd, SPI_IOC_RD_BITS_PER_WORD,&spiBPW)< 0){
        printf("Can't get bits per word:%s \r\n",strerror(errno));
        return -1;
    }

/*
 *  设置 spi 读写速度
 */
    if(ioctl(fd,SPI_IOC_WR_MAX_SPEED_HZ,&speed) < 0){
        printf("Can't set max speed hz:%s \r\n",strerror(errno));
        return -1;
    }
    if(ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ,&speed) < 0){
        printf("Can't get max speed hz:%s \r\n",strerror(errno));
        return -1;
    }
    
    return fd; 
}


/*
 * SPIOpen
 *    Open the SPI device and set it up ,etc , in the default MODE 0 
 *
 */

int SPIOpen(int channel, int speed)
{
    return SPISetupMode(channel, speed, 0);
}

/*
 * SPIClose 
 *  Close the SPI device 
 */

int SPIClose(int channel)
{
    int ret;
    if(channel >= 2)
        return -1;
    ret = close(spiFds[channel]);
    if(ret < 0){
        printf("close spidev%d fail, %s \r\n",channel, strerror(errno));
        return -1;
    }
    return 0;
}
