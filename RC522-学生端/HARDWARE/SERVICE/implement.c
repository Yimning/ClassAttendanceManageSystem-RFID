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
char SignedUpFlag=1;
u8 ReadCard()
{
	u8 status=MI_ERR;
	snr=1;           // 写入扇区1
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
			printf("%02X%02X%02X%02X%X\r\n",SN[0],SN[1],SN[2],SN[3],SignedUpFlag);  //%02X可以补0
		}
		
	  status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SN);    // 校验1扇区密码，密码位于每一扇区第3块
                                                          	// 验证A密钥，块地址，扇区密码，卡序列号			
   if(status==MI_OK)//认证成功
		{
      ReadSnr();
		}		
		
	}
	else
	{
		PcdAntennaOff();  //关闭天线
	}
}

	u8 status=MI_ERR; u8 flag;	
	u16 t; 
	u16 len;	u8 key=0;
u8 ReadSnr()
{
			status = PcdRead((snr*4+0), buf);   // 读卡，读取1扇区0块数据到buf[0]-buf[16]
			if(status==MI_OK) 
			 {
					//读写成功
				//	printf("read finish!\n");
				//	printf("读到的值是： %s\n",data);
			  OLED_ShowString(0,52,buf,12);  
				//	printf("%s\n",buf);
				 
			 }
			 else 
			 {
				 // printf("read error!\n")
				  OLED_ShowString(0,52,"error",12);
			 };

}


