#include "rc522pin.h"
#include "delay.h"
#include "sys.h"
#include "rc522.h"
#include "lcd.h"			 //��ʾģ��
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

int char_array_cmp(unsigned char*cmd,int len,unsigned char*checkcode,int length);

int main(void)
{	

	unsigned char status;
	u8 key=0;
	u16 t; 
	u16 len;
	int result; 
	int flag;
	unsigned char cmd[16];    //���ݻ�������
	unsigned char checkcode[16]="s201"; 
	unsigned char checkcode1[16]="s202"; 
	unsigned char checkcode2[16]="over"; 
	
	NVIC_PriorityGroupConfig(2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
 	delay_init();	    	 //��ʱ������ʼ��	
 	RC522Pin_Init();			 //���ģ���ʼ��
	uart_init(115200);
  RELAY_Init();
	BEEP_Init();
	
	KEY_Init();
	InitRc522();				//��ʼ����Ƶ��ģ��
	RTC_Init();	  			//RTC��ʼ��	
	OLED_Init();			//��ʼ��OLED       
	OLED_ShowString(0,0,"ClassAttendence",16);  
	//printf("��key0��ȡ����\r\n");
	OLED_Refresh_Gram();		//������ʾ��OLED 
	
	while(1)
	{
	   displaytime(); 
		//while(!(USART_RX_STA&0x8000));	 //�ȴ�����
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
		//	printf("\r\n����������Ϊ:");
			for(t=0;t<len;t++)
			{
				cmd[t]=USART_RX_BUF[t];
			}
		  USART_RX_STA=0;	  //��ջ�����
     // printf("%s",cmd);
		}
		
		result=char_array_cmp(cmd,len,checkcode,4);
    if (result == 0)
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
		
		result=char_array_cmp(cmd,len,checkcode1,4);
		if (result == 0)
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
		int lmin=len>length?length:len;//��С�����ߡ�
		int i;
		for(i=0;i<lmin;i++)
		if(cmd[i]>checkcode[i])return 1;
		else if(cmd[i]<checkcode[i]) return -1;

		//���е������ʾ�������ж�Ԫ�ؾ���ȡ�
		if(len==length)return 0;
		if(len>length)return 1;
		return -1;//l1<l2�������
}

