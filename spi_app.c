/*************************************************************************
   Copyright: (C) 2019 xinkui.fu  
   All rights reserved. 
   File Name: spi_app.c
   Description: 
   
   Author: 
   Mail: 
   Date: Mon 18 Feb 2019 06:41:04 UTC
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "SPISet.h"
#include <unistd.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#define START_FLAG          0x00000000
#define END_FLAG            0xffffffff
#define SUBDEV_NUM          3

typedef struct{
    unsigned char global:5;
    unsigned char msb:3;
    unsigned char blue;
    unsigned char green;
    unsigned char red;
}drv_info_t;


typedef struct {
    unsigned int start;
    drv_info_t   info[SUBDEV_NUM];
    unsigned int end;
}led_dev_t;

int initSPI(void)
{
    int spifd;
    spifd = SPIOpen(0,500000);
    if(spifd == -1){
        printf("init spi fail \r\n");
        return -1;
    }
    return 0;
}

int deinitSPI(void)
{
    int ret;
    ret = SPIClose(0);
    if(ret == -1){
        printf("Deinit spi fail \r\n");
        return -1;
    }
    return 0;
}


led_dev_t dev;

void dev_init(led_dev_t *dev)
{
    int i;
    if(dev == NULL)
        return ;
    memset(dev,0,sizeof(led_dev_t));
    dev->start = START_FLAG;
    dev->end = END_FLAG;
    for(i = 0; i < SUBDEV_NUM; i++){
        dev->info[i].msb = 7;
        dev->info[i].global = 31;
        dev->info[i].green = 255;
    }

}

int dev_set_val(unsigned char id,unsigned char brightness,unsigned char blue,unsigned char green,unsigned red)
{
    int i;
    drv_info_t *drv;
    if(id >= SUBDEV_NUM)
        return -1;
    if(brightness > 31)
        brightness = 31;
    drv = &dev.info[id];
    drv->global = brightness;
    drv->green = green;
    drv->blue = blue;
    drv->red = red;
    return 0;
}

int dev_set_red(unsigned char id,unsigned char brightness,unsigned char red)
{
    drv_info_t *drv; 
    if(id > SUBDEV_NUM)
        return -1;
    if(brightness > 31)
        brightness = 31;
    drv = &dev.info[id];
    drv->global = brightness;
    drv->green = 0;
    drv->blue = 0;
    drv->red = red;
    return 0;
}

int dev_set_blue(unsigned char id,unsigned char brightness,unsigned char val)
{
    drv_info_t *drv; 
    if(id > SUBDEV_NUM)
        return -1;
    if(brightness > 31)
        brightness = 31;
    drv = &dev.info[id];
    drv->global = brightness;
    drv->green = 0;
    drv->blue = val;
    drv->red = 0;
    return 0;
}

int dev_set_green(unsigned char id,unsigned char brightness,unsigned char val)
{
    drv_info_t *drv; 
    if(id > SUBDEV_NUM)
        return -1;
    if(brightness > 31)
        brightness = 31;
    drv = &dev.info[id];
    drv->global = brightness;
    drv->green = val;
    drv->blue = 0;
    drv->red = 0;
    return 0;
}

void dm_task_sleep(unsigned long ms)
{
    struct timeval timeout;
    timeout.tv_sec = ms/1000;
    timeout.tv_usec = (ms - timeout.tv_sec*1000)*1000;
    printf("sleep %d us \r\n",timeout.tv_usec);
    select(0,NULL,NULL,NULL,&timeout);
}
int dev_set_split(void)
{
    drv_info_t *drv =&dev.info[0];
	drv_info_t *drvn;
	drv_info_t snap;
	int i;
	memcpy(&snap,drv,sizeof(drv_info_t));
	for(i = 0;i < (SUBDEV_NUM-1); i++){
		drv = &dev.info[i];
		drvn = &dev.info[i+1];
		memcpy(drv,drvn,sizeof(drv_info_t));
	}
	memcpy(&dev.info[SUBDEV_NUM-1],&snap,sizeof(drv_info_t));
    return 0;
}
int main(void)
{
    unsigned int cnt = 0;
	int state = 0;
    unsigned char rx_data[100];
    initSPI();
    dev_init(&dev);
	dev_set_val(0,31,255,0,0);
	dev_set_val(1,31,0,128,0);
	dev_set_val(2,31,0,0,32);
    while(1){
		dev_set_split();
        SPIDataRW(0,(unsigned char *)&dev,NULL,sizeof(dev)); // write spi bus 7 byte data;
        //sleep(1);
		printf("split led ...\r\n");
		dm_task_sleep(200);
		cnt ++;
    }
    deinitSPI();
    return 0;
}
