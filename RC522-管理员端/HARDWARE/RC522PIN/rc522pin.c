#include "rc522pin.h"
//*����˵����
//*1--SS  <----->PF13   ���츴�����
//*2--SCK <----->PB13  ���츴�����
//*3--MOSI<----->PB15  ���츴�����
//*4--MISO<----->PB14  ���츴�����
//*5--����
//*6--GND <----->GND   
//*7--RST <----->PF14    ���츴�����
//*8--VCC <----->VCC
//////////////////////////////////////////////////////////////////////////////////	 

//���������������	   
									  
////////////////////////////////////////////////////////////////////////////////// 	   

void RC522Pin_Init(void)
{	
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF,ENABLE);//ʹ��ʱ��	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     // ���츴�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz	
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_14;  //Pin0ΪSPI2��Ƭѡ�ˣ�ʹ�ܣ�����ͬͨ��ʱд0 
                                                        	//Pin1ΪRC522��λ���� ; ����Ҫ�޸���������Ҫע��rc522.h
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		    //IO���ٶ�Ϊ50MHz	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	    // ���츴�����  ������������ܲ���
 	GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
}
 
