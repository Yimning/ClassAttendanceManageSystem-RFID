#include "rc522.h"
void delay_ns(u32 ns)
{
  u32 i;
  for(i=0;i<ns;i++)
  {
    __nop();
    __nop();
    __nop();
  }
}
/*************************************************************
funcname:InitRc522
parameter:void
return:void
function: RC522初始化
Author:Mr.Wang
Date:2018-5-12 11:31:14
**************************************************************/

void InitRc522(void)
{
  SPI2_Init();
  SPI2_SpeedSet(7); //APB1的256分频
  PcdReset();
  PcdAntennaOff();
  delay_ms(2);  
  PcdAntennaOn();
  M500PcdConfigISOType( 'A' );
}
/*************************************************************
funcname:Reset_RC522
parameter:void
return:void
function: RC522复位
Author:Mr.Wang
Date:2018-5-12 11:31:14
**************************************************************/
void Reset_RC522(void)
{
  PcdReset();
  PcdAntennaOff();
  delay_ms(2);  
  PcdAntennaOn();
}                         
/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(u8   req_code,u8 *pTagType)            //读卡请求
{
	char   status;    
	u8   unLen;
	u8   ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);  //
	WriteRawRC(BitFramingReg,0x07); //定义发送的最后一个字节的位数：7位  111
	SetBitMask(TxControlReg,0x03);  //开启天线
 
	ucComMF522Buf[0] = req_code; //发送M1卡片命令字

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

	if ((status == MI_OK) && (unLen == 0x10))  //unLen为接收到的位数，0x10为16，即两个字节
	{    
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{   status = MI_ERR;   }
   
	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(u8 *pSnr)
{
    char   status;
    u8   i,snr_check=0;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 
    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);  //禁止发送位
    ClearBitMask(CollReg,0x80); //
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;  //卡片防冲突命令
    ucComMF522Buf[1] = 0x20;  //在ISO14443A协议中介绍为什么要0x20
	
	//返回被选卡的序列号
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);
	//printf("unLen:%d\r\n",unLen);  返回5个字节，最后一个作为校验
    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
			// printf("pSnr[%d]=%d  snr_check=%d\r\n",i,pSnr[i],snr_check);
         }
		 //printf("ucComMF522Buf[4]=%d\r\n",ucComMF522Buf[4]);  调试即可得到结果
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(u8 *pSnr)
{
    char   status;
    u8   i;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 
	u8   reciveBuf[MAXRLEN];
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70; 
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]); //CRC校验，并将两字节校验值放在ucComMF522Buf[7]、ucComMF522Buf[8]
  
    ClearBitMask(Status2Reg,0x08);
	//选择被选中的卡的序列号，并同时返回卡的容量代码(SAK)。
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,reciveBuf,&unLen);
    //printf("%d,%X %X %X\r\n",unLen,reciveBuf[0],reciveBuf[1],reciveBuf[2]);
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////               
char PcdAuthState(u8   auth_mode,u8   addr,u8 *pKey,u8 *pSnr)
{
    char   status;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;  
    ucComMF522Buf[1] = addr;
    memcpy(&ucComMF522Buf[2], pKey, 6);   // 
    memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          p [OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
///////////////////////////////////////////////////////////////////// 
char PcdRead(u8   addr,u8 *p )
{
    char   status;
    u8   unLen;
    u8   i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
    {
        for (i=0; i<16; i++)
        {    
					*(p +i) = ucComMF522Buf[i];
		    }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          p [IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                  
char PcdWrite(u8   addr,u8 *p )
{
    char   status;
    u8   unLen;
    u8   i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, p , 16);
        for (i=0; i<16; i++)
        {    
        	ucComMF522Buf[i] = *(p +i);   
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) )//|| ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    //u8   status=0;
    //u8   unLen;
    u8   ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    //status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(u8 *pIn ,u8   len,u8 *pOut )
{
    u8   i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIn +i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }while ((i!=0) && !(n&0x04));
    pOut [0] = ReadRawRC(CRCResultRegL);
    pOut [1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{

	SET_RC522RST;
    delay_ns(100);
	CLR_RC522RST;
    delay_ns(100);
	SET_RC522RST;
    delay_ns(100);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
	WriteRawRC(CommandReg,PCD_RESETPHASE);
    delay_ns(10);
    
    WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);
	
	WriteRawRC(TxAutoReg,0x40);//必须要
   
    return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//设置RC632的工作方式 
//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(u8   type)
{
   if (type == 'A')                     //ISO14443_A
   { 
		 ClearBitMask(Status2Reg,0x08);
		 WriteRawRC(ModeReg,0x3D);//3F
		 WriteRawRC(RxSelReg,0x86);//84
		 WriteRawRC(RFCfgReg,0x7F);   //4F
		 WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
	   WriteRawRC(TReloadRegH,0);
     WriteRawRC(TModeReg,0x8D);
	   WriteRawRC(TPrescalerReg,0x3E);
	   delay_ns(1000);
		 PcdAntennaOn();
   }
   else{ return 1; }
   
   return MI_OK;
}
/////////////////////////////////////////////////////////////////////
//功    能：读RC522寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
u8 ReadRawRC(u8   Address)
{
    u8   ucAddr;
    u8   ucResult=0;
	CLR_SPI_CS;
    ucAddr = ((Address<<1)&0x7E)|0x80;
	
	SPIWriteByte(ucAddr);
	ucResult=SPIWriteByte(0);
	SET_SPI_CS;
   return ucResult;
}

/////////////////////////////////////////////////////////////////////
//功    能：写RC522寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(u8   Address, u8   value)
{  
    u8   ucAddr;
	CLR_SPI_CS;
    ucAddr = ((Address<<1)&0x7E);
	SPIWriteByte(ucAddr);
	SPIWriteByte(value);
	SET_SPI_CS;
}
/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(u8   reg,u8   mask)  
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(u8   reg,u8   mask)  
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pIn [IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOut [OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(u8   Command, 
                 u8 *pIn , 
                 u8   InLenByte,
                 u8 *pOut , 
                 u8 *pOutLenBit)
{
    char   status = MI_ERR;   //返回状态
    u8   irqEn   = 0x00;
    u8   waitFor = 0x00;
    u8   lastBits;
    u8   n;
    u16   i;
    switch (Command)
    {
        case PCD_AUTHENT:   //验证秘钥
			irqEn   = 0x12;
			waitFor = 0x10;
			break;
		case PCD_TRANSCEIVE:  //发送接收命令
			irqEn   = 0x77;
			waitFor = 0x30;
			break;
		default:
			break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);		//清所有中断位
    WriteRawRC(CommandReg,PCD_IDLE); 	//取消当前命令
    SetBitMask(FIFOLevelReg,0x80);	 	//清FIFO缓存
    
    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pIn [i]);    }  //写数据到FIFO
    WriteRawRC(CommandReg, Command);    //发送命令字：PCD_TRANSCEIVE（发送并接收数据） PCD_AUTHENT（验证秘钥）	  
//   	 n = ReadRawRC(CommandReg);
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }	 //开始传送
    										 
    //i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
	i = 2000;
    do 
    {
        n = ReadRawRC(ComIrqReg);
        i--;
    }while ((i!=0) && !(n&0x01) && !(n&waitFor)); //ComIrqReg的某些位已经被置1，退出循环
    ClearBitMask(BitFramingReg,0x80);   //停止传送
    if (i!=0)  //没有超出最长等待时间
    {    
        if(!(ReadRawRC(ErrorReg)&0x1B)) //ErrorReg的位4,3,1,0不置位，说明相关错误没有发生
        {
            status = MI_OK;  //没有错误返回则将状态标志成功
            if (n & irqEn & 0x01)   //定时器标志置1且发送的指令为发送接收数据，说明减到0，超时
            {   status = MI_NOTAGERR;   }  //将状态标志置为MI_NOTAGERR
            if (Command == PCD_TRANSCEIVE) //若发送指令为验证秘钥
            {
               	n = ReadRawRC(FIFOLevelReg);   //指示 FIFO 缓冲区中保存的字节数
              	lastBits = ReadRawRC(ControlReg) & 0x07; //显示最后接收到的字节的有效位的数目
                if (lastBits)   //最后字节接收有效值不为0
                {   *pOutLenBit = (n-1)*8 + lastBits;   }   //返回的数据长度
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOut [i] = ReadRawRC(FIFODataReg);    }
            }
        }
        else
        {   status = MI_ERR;   }
        
    }
    SetBitMask(ControlReg,0x80);           // stop timer now
    WriteRawRC(CommandReg,PCD_IDLE); //取消当前命令
    return status;
}

/////////////////////////////////////////////////////////////////////
//开启天线  
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
    u8   i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}
/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////                 
char PcdValue(u8 dd_mode,u8 addr,u8 *pValue)
{
    char status;
    u8  unLen;
    u8 ucComMF522Buf[MAXRLEN]; 
    //u8 i;
    ucComMF522Buf[0] = dd_mode;  //充值|扣款命令
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);  //校验
	
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	  //printf("%d %X %X %X\r\n",unLen,ucComMF522Buf[0],ucComMF522Buf[1],ucComMF522Buf[2]);
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
		{   
			status = MI_ERR; 
		printf("ucComMF522Buf[0]:%d\r\n",ucComMF522Buf[0]); 
		}  
    if (status == MI_OK)  //校验成功可发送充值|扣款金额
    {
        memcpy(ucComMF522Buf, pValue, 4);  //充值|扣款金额
        //for (i=0; i<16; i++)
        //{    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);//校验
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;  //保存缓冲区数据
        ucComMF522Buf[1] = addr;   
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);   //校验
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR; printf("err1\r\n");   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(u8 sourceaddr, u8 goaladdr)
{
    char status;
    u8  unLen;
    u8 ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    return MI_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}
/*************************************
*函数功能：显示卡的卡号，以十六进制显示
*u8* PSnr_16：存储十六进制的数组卡号
*	u8* PSnr：需要转换的数组卡号
***************************************/
void ShowID(u8* PSnr_16,u8* PSnr)	 //显示卡的卡号，以十六进制显示
{
	u8 i=0;
	for(i=0;i<4;i++)
	{
		sprintf((char*)&PSnr_16[i*2], "%x", PSnr[i]);
	}
}


