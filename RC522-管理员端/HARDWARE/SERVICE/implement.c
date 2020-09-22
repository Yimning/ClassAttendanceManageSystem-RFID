#include "implement.h"
#include "oled.h"
unsigned char CT[2];//卡类型
unsigned char SN[4]; //卡号 
unsigned char snr1[2]; //代写区号 
unsigned char buf[16];   //自定义扇区0-15  每个扇区分为四个块
unsigned char data[16];    //代写数据数组
unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};    //工厂初始化密码
u8 snr;
char cmd;
u8 status=MI_ERR; u8 flag;	
u16 t; 
u16 len;	u8 key=0;
u8 ReadCard()
{
	u8 status=MI_ERR;
	PcdAntennaOn(); //开启天线
	do
	{
		status=PcdRequest(PICC_REQALL,CT);  
	}
	while(status!=MI_OK); //等待寻卡成功
	if(status==MI_OK)  //寻卡成功
	{
		status=MI_ERR;
		status = PcdAnticoll(SN);/*防冲撞*/
		//printf("寻卡成功！\r\n");
		if (status==MI_OK)//防冲撞成功
		{
			status=MI_ERR;
			status=PcdSelect(SN); /*选卡*/
			//printf("防碰撞成功！\r\n");
		}
		if(status==MI_OK)//选卡成功
		{
			status=MI_ERR;
			//printf("选卡成功！\r\n");
			printf("卡号:%02X%02X%02X%02X\r\n",SN[0],SN[1],SN[2],SN[3]);
		}
	}
	else
	{
		PcdAntennaOff();  //关闭天线
	}
}



u8 WriteCard()
{

	 // printf("进入读写操作...\r\n");
		//printf("卡号:%02X%02X%02X%02X\r\n",SN[0],SN[1],SN[2],SN[3]);
		
		if(!flag)  // 认证
		{
		//printf("请输入扇区01-15\r\n");				 
		while(!(USART_RX_STA&0x8000));	 
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			//printf("\r\n您输入的扇区为:");
			for(t=0;t<2;t++)
			{
				snr1[t]=USART_RX_BUF[t];
//				USART_SendData(USART1, USART_RX_BUF[t]);//向串口1发送数据
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
			//	printf("%s",snr1);
			 OLED_ShowString(0,52,snr1,12);
				USART_RX_STA=0;
		 }	
	  	snr=(snr1[0]-'0')*10+snr1[1]-'0';
			//snr = 1;    // 扇区号1			 

			status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SN);    // 校验1扇区密码，密码位于每一扇区第3块
                                                          	// 验证A密钥，块地址，扇区密码，卡序列号
		 if(status==MI_OK)//认证成功
		 {
			 status=MI_ERR;
			//	printf("\r\n认证成功！\r\n");		
				 //printf("卡号:%X %X %X %X\r\n",SN[0],SN[1],SN[2],SN[3]);
				 
			//	 printf("\r\n选择当前操作命令，发w写卡；发r读卡！！！\r\n\r\n");
				while(!(USART_RX_STA&0x8000));	 //等待接收
				if(USART_RX_STA&0x8000)
				{					   
					len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
					//printf("\r\n您发的命令为:");
					for(t=0;t<1;t++)
					{
						cmd=USART_RX_BUF[t];
						//printf("%c",cmd);
					}
					USART_RX_STA=0;	  //清空缓冲区
				}
			}
    switch(cmd)
		{
     case  'w':   
          WriteSnr();
			    break; 
	   case  'r':     	//读卡
          ReadSnr();
	 	      break; 
		 default:
		     OLED_ShowString(0,52,"error!",12);
		     break;
	    }	
	 }
	 else 	
		{
        //printf("认证失败!");			 
				PcdAntennaOff();  //关闭天线	
		}
}

u8 WriteSnr()
	{
    // printf("\r\n请输入写卡内容\r\n");	
	  while(!(USART_RX_STA&0x8000));	 
			if(USART_RX_STA&0x8000)
	  	{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			//printf("\r\n您写卡的内容为:\r\n\r\n");
			for(t=0;t<len;t++)
			{
				data[t]=USART_RX_BUF[t];
//				USART_SendData(USART1, USART_RX_BUF[t]);//向串口1发送数据
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		     printf("%c",USART_RX_BUF[t]);
			}
				//printf("\r\n\r\n");//插入换行
				USART_RX_STA=0;
	  	}
			
			status = PcdWrite((snr*4+0), data);   //写卡，将buf[0]-buf[16]写入1扇区0块
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
	
		status = PcdRead((snr*4+0), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16]
		if(status==MI_OK) 
		 {
				//读写成功
			//	printf("read finish!\n");
			 //printf("读到的值是： %s\n",buf);
				OLED_ShowString(0,52,buf,12);

		 }else 
		 {
			 OLED_ShowString(0,52,"read error!",12);
		 }
}


