#ifndef _STORE_H
#define _STORE_H
#include "sys.h"
#include "rc522.h"
#include "key.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"
extern unsigned char CT[2];//ø®¿‡–Õ
extern unsigned char SN[4]; //ø®∫≈
u8 ReadCard();  //∂¡ø®
u8 WriteCard();  //–¥ø®
u8 WriteSnr();
u8 ReadSnr();

#endif


