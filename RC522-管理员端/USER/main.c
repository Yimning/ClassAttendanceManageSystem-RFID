#include "rc522pin.h"
#include "delay.h"
#include "sys.h"
#include "rc522.h"
//#include "lcd.h"			 //��ʾģ��
#include "key.h"       
#include "usart.h"
#include "string.h" 
#include "led.h"
#include "implement.h"
#include "rtc.h" 
#include "oled.h"
#include "displaytime.h"	
#include "relay.h"	
#include "beep.h"	
/*******************************
*����˵����
*1--SS  <----->PF13
*2--SCK <----->PB13
*3--MOSI<----->PB15
*4--MISO<----->PB14
*5--����
*6--GND <----->GND
*7--RST <----->PF14
*8--VCC <----->VCC
************************************/
int main(void)
{		
	unsigned char status;
	u8 key=0;
//	Stm32_Clock_Init(9);
	NVIC_PriorityGroupConfig(2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
 	delay_init();	    	 //��ʱ������ʼ��	
 	RC522Pin_Init();			 //���ģ���ʼ��
	uart_init(115200);
	//LCD_Init();
	KEY_Init();
	InitRc522();				//��ʼ����Ƶ��ģ��
  RELAY_Init();
	BEEP_Init();	
	RTC_Init();	  			//RTC��ʼ��	
	OLED_Init();			//��ʼ��OLED   
	OLED_ShowString(0,0,"ClassAttendence",16);  
	//printf("��key0��ȡ����\r\n");
	OLED_Refresh_Gram();		//������ʾ��OLED 	
	

	//printf("��key0��ȡ����\r\n");
	while(1)
	{
	  	displaytime(); 
			key=KEY_Scan(0);
	  	if(key==1) 
			{
		   ReadCard();	
  		 BEEP=1;   
			 delay_ms(100);//��ʱ300ms 
			 BEEP=0;		
			 delay_ms(100);//��ʱ300ms
			 RELAY = 0 ;
			 delay_ms(300);//��ʱ300ms
			 RELAY = 1 ;
    	}
		  if(key==2) 
			{
				WriteCard();
    	}
	}
}
			


