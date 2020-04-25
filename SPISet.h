/*************************************************************************
   Copyright: (C) 2019 xinkui.fu  
   All rights reserved. 
   File Name: SPISet.h
   Description: 
   
   Author: 
   Mail: 
   Date: Mon 18 Feb 2019 07:24:56 UTC
 ************************************************************************/

#ifndef _SPISET_H
#define _SPISET_H
#ifdef __cplusplus
extern "C" {
#endif


int SPIDataRW(int channel, unsigned char *tx_data, unsigned char *rx_data, int len);
int SPISetupMode(int channel, int speed, int mode);
int SPIOpen(int channel, int speed);
int SPIClose(int channel);

#ifdef __cplusplus
}
#endif

#endif
