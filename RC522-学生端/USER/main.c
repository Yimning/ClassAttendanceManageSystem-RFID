#include "rc522pin.h"
#include "delay.h"
#include "sys.h"
#include "rc522.h"
#include "lcd.h"			 //显示模块
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

int char_array_cmp(unsigned char*cmd,int len,unsigned char*checkcode,int length);

int main(void)
{	

	unsigned char status;
	u8 key=0;
	u16 t; 
	u16 len;
	int result; 
	int flag;
	unsigned char cmd[16];    //数据缓冲数组
	unsigned char checkcode[16]="s201"; 
	unsigned char checkcode1[16]="s202"; 
	unsigned char checkcode2[16]="over"; 
	
	NVIC_PriorityGroupConfig(2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
 	delay_init();	    	 //延时函数初始化	
 	RC522Pin_Init();			 //输出模块初始化
	uart_init(115200);
  RELAY_Init();
	BEEP_Init();
	
	KEY_Init();
	InitRc522();				//初始化射频卡模块
	RTC_Init();	  			//RTC初始化	
	OLED_Init();			//初始化OLED       
	OLED_ShowString(0,0,"ClassAttendence",16);  
	//printf("按key0获取卡号\r\n");
	OLED_Refresh_Gram();		//更新显示到OLED 
	
	while(1)
	{
	   displaytime(); 
		//while(!(USART_RX_STA&0x8000));	 //等待接收
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
		//	printf("\r\n您发的命令为:");
			for(t=0;t<len;t++)
			{
				cmd[t]=USART_RX_BUF[t];
			}
		  USART_RX_STA=0;	  //清空缓冲区
     // printf("%s",cmd);
		}
		
		result=char_array_cmp(cmd,len,checkcode,4);
    if (result == 0)
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
		
		result=char_array_cmp(cmd,len,checkcode1,4);
		if (result == 0)
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
			
		result=char_array_cmp(cmd,len,checkcode2,4);
		if (result == 0)
		{
			 displaytime();
         break ;
		}
	}
	
	
}

int char_array_cmp(unsigned char*cmd,int len,unsigned char*checkcode,int length)
{
		int lmin=len>length?length:len;//较小长度者。
		int i;
		for(i=0;i<lmin;i++)
		if(cmd[i]>checkcode[i])return 1;
		else if(cmd[i]<checkcode[i]) return -1;

		//运行到这里，表示所有已判断元素均相等。
		if(len==length)return 0;
		if(len>length)return 1;
		return -1;//l1<l2的情况。
}

