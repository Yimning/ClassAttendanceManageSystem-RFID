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
char SignedUpFlag=1;
u8 ReadCard()
{
	u8 status=MI_ERR;
	snr=1;           // д������1
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
			printf("%02X%02X%02X%02X%X\r\n",SN[0],SN[1],SN[2],SN[3],SignedUpFlag);  //%02X���Բ�0
		}
		
	  status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SN);    // У��1�������룬����λ��ÿһ������3��
                                                          	// ��֤A��Կ�����ַ���������룬�����к�			
   if(status==MI_OK)//��֤�ɹ�
		{
      ReadSnr();
		}		
		
	}
	else
	{
		PcdAntennaOff();  //�ر�����
	}
}

	u8 status=MI_ERR; u8 flag;	
	u16 t; 
	u16 len;	u8 key=0;
u8 ReadSnr()
{
			status = PcdRead((snr*4+0), buf);   // ��������ȡ1����0�����ݵ�buf[0]-buf[16]
			if(status==MI_OK) 
			 {
					//��д�ɹ�
				//	printf("read finish!\n");
				//	printf("������ֵ�ǣ� %s\n",data);
			  OLED_ShowString(0,52,buf,12);  
				//	printf("%s\n",buf);
				 
			 }
			 else 
			 {
				 // printf("read error!\n")
				  OLED_ShowString(0,52,"error",12);
			 };

}


