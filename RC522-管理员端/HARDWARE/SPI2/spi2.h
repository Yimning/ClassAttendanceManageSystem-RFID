#ifndef _SPI2_H
#define _SPI2_H

#include "sys.h"
void SPI2_Init(void); //SPI初始化
void SPI2_SpeedSet(u8 speed); //SPI时钟设置，来自APB1的分频
u8 SPI2_ReadWriteByte(u8 TxData);//SPI读写函数
u8 SPIWriteByte(u8 Byte); //SPI发送
#endif
