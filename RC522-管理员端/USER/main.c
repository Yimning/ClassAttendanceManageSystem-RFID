#include "rc522pin.h"
#include "delay.h"
#include "sys.h"
#include "rc522.h"
//#include "lcd.h"			 //显示模块
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
*连线说明：
*1--SS  <----->PF13
*2--SCK <----->PB13
*3--MOSI<----->PB15
*4--MISO<----->PB14
*5--悬空
*6--GND <----->GND
*7--RST <----->PF14
*8--VCC <----->VCC
************************************/
int main(void)
{		
	unsigned char status;
	u8 key=0;
//	Stm32_Clock_Init(9);
	NVIC_PriorityGroupConfig(2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
 	delay_init();	    	 //延时函数初始化	
 	RC522Pin_Init();			 //输出模块初始化
	uart_init(115200);
	//LCD_Init();
	KEY_Init();
	InitRc522();				//初始化射频卡模块
  RELAY_Init();
	BEEP_Init();	
	RTC_Init();	  			//RTC初始化	
	OLED_Init();			//初始化OLED   
	OLED_ShowString(0,0,"ClassAttendence",16);  
	//printf("按key0获取卡号\r\n");
	OLED_Refresh_Gram();		//更新显示到OLED 	
	

	//printf("按key0获取卡号\r\n");
	while(1)
	{
	  	displaytime(); 
			key=KEY_Scan(0);
	  	if(key==1) 
			{
		   ReadCard();	
  		 BEEP=1;   
			 delay_ms(100);//延时300ms 
			 BEEP=0;		
			 delay_ms(100);//延时300ms
			 RELAY = 0 ;
			 delay_ms(300);//延时300ms
			 RELAY = 1 ;
    	}
		  if(key==2) 
			{
				WriteCard();
    	}
	}
}
			


