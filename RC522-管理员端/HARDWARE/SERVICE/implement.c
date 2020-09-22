#include "implement.h"
#include "oled.h"
unsigned char CT[2];//������
unsigned char SN[4]; //���� 
unsigned char snr1[2]; //��д���� 
unsigned char buf[16];   //�Զ�������0-15  ÿ��������Ϊ�ĸ���
unsigned char data[16];    //��д��������
unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};    //������ʼ������
u8 snr;
char cmd;
u8 status=MI_ERR; u8 flag;	
u16 t; 
u16 len;	u8 key=0;
u8 ReadCard()
{
	u8 status=MI_ERR;
	PcdAntennaOn(); //��������
	do
	{
		status=PcdRequest(PICC_REQALL,CT);  
	}
	while(status!=MI_OK); //�ȴ�Ѱ���ɹ�
	if(status==MI_OK)  //Ѱ���ɹ�
	{
		status=MI_ERR;
		status = PcdAnticoll(SN);/*����ײ*/
		//printf("Ѱ���ɹ���\r\n");
		if (status==MI_OK)//����ײ�ɹ�
		{
			status=MI_ERR;
			status=PcdSelect(SN); /*ѡ��*/
			//printf("����ײ�ɹ���\r\n");
		}
		if(status==MI_OK)//ѡ���ɹ�
		{
			status=MI_ERR;
			//printf("ѡ���ɹ���\r\n");
			printf("����:%02X%02X%02X%02X\r\n",SN[0],SN[1],SN[2],SN[3]);
		}
	}
	else
	{
		PcdAntennaOff();  //�ر�����
	}
}



u8 WriteCard()
{

	 // printf("�����д����...\r\n");
		//printf("����:%02X%02X%02X%02X\r\n",SN[0],SN[1],SN[2],SN[3]);
		
		if(!flag)  // ��֤
		{
		//printf("����������01-15\r\n");				 
		while(!(USART_RX_STA&0x8000));	 
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			//printf("\r\n�����������Ϊ:");
			for(t=0;t<2;t++)
			{
				snr1[t]=USART_RX_BUF[t];
//				USART_SendData(USART1, USART_RX_BUF[t]);//�򴮿�1��������
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
			//	printf("%s",snr1);
			 OLED_ShowString(0,52,snr1,12);
				USART_RX_STA=0;
		 }	
	  	snr=(snr1[0]-'0')*10+snr1[1]-'0';
			//snr = 1;    // ������1			 

			status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SN);    // У��1�������룬����λ��ÿһ������3��
                                                          	// ��֤A��Կ�����ַ���������룬�����к�
		 if(status==MI_OK)//��֤�ɹ�
		 {
			 status=MI_ERR;
			//	printf("\r\n��֤�ɹ���\r\n");		
				 //printf("����:%X %X %X %X\r\n",SN[0],SN[1],SN[2],SN[3]);
				 
			//	 printf("\r\nѡ��ǰ���������wд������r����������\r\n\r\n");
				while(!(USART_RX_STA&0x8000));	 //�ȴ�����
				if(USART_RX_STA&0x8000)
				{					   
					len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
					//printf("\r\n����������Ϊ:");
					for(t=0;t<1;t++)
					{
						cmd=USART_RX_BUF[t];
						//printf("%c",cmd);
					}
					USART_RX_STA=0;	  //��ջ�����
				}
			}
    switch(cmd)
		{
     case  'w':   
          WriteSnr();
			    break; 
	   case  'r':     	//����
          ReadSnr();
	 	      break; 
		 default:
		     OLED_ShowString(0,52,"error!",12);
		     break;
	    }	
	 }
	 else 	
		{
        //printf("��֤ʧ��!");			 
				PcdAntennaOff();  //�ر�����	
		}
}

u8 WriteSnr()
	{
    // printf("\r\n������д������\r\n");	
	  while(!(USART_RX_STA&0x8000));	 
			if(USART_RX_STA&0x8000)
	  	{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			//printf("\r\n��д��������Ϊ:\r\n\r\n");
			for(t=0;t<len;t++)
			{
				data[t]=USART_RX_BUF[t];
//				USART_SendData(USART1, USART_RX_BUF[t]);//�򴮿�1��������
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		     printf("%c",USART_RX_BUF[t]);
			}
				//printf("\r\n\r\n");//���뻻��
				USART_RX_STA=0;
	  	}
			
			status = PcdWrite((snr*4+0), data);   //д������buf[0]-buf[16]д��1����0��
			if(status==MI_OK)  		
			{  
				//printf("write finish!\n");
				 OLED_ShowString(0,52,"write finish!",12);
			}
			else  
				{
				//printf("write error!\n");
					 OLED_ShowString(0,52,"write error!",12);
				}
		}
	
u8 ReadSnr()
{
	
		status = PcdRead((snr*4+0), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
		if(status==MI_OK) 
		 {
				//��д�ɹ�
			//	printf("read finish!\n");
			 //printf("������ֵ�ǣ� %s\n",buf);
				OLED_ShowString(0,52,buf,12);

		 }else 
		 {
			 OLED_ShowString(0,52,"read error!",12);
		 }
}


