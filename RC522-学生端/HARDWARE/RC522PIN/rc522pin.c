#include "rc522pin.h"
//*连线说明：
//*1--SS  <----->PF13   推挽复用输出
//*2--SCK <----->PB13  推挽复用输出
//*3--MOSI<----->PB15  推挽复用输出
//*4--MISO<----->PB14  推挽复用输出
//*5--悬空
//*6--GND <----->GND   
//*7--RST <----->PF14    推挽复用输出
//*8--VCC <----->VCC
//////////////////////////////////////////////////////////////////////////////////	 

//输出输入驱动代码	   
									  
////////////////////////////////////////////////////////////////////////////////// 	   

void RC522Pin_Init(void)
{	
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF,ENABLE);//使能时钟	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     // 推挽复用输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz	
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_14;  //Pin0为SPI2的片选端（使能），不同通信时写0 
                                                        	//Pin1为RC522复位引脚 ; 若需要修改引脚则需要注意rc522.h
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		    //IO口速度为50MHz	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	    // 推挽复用输出  ，浮空输入可能不行
 	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
}
 
