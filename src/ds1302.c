#include "ds1302.h"
//端口定义
sbit SCK=P1^0;
sbit SDA=P1^1;
sbit RST=P1^2;

//unsigned char time_buf_pre[8] = {0x20,0x10,0x12,0x31,0x23,0x59,0x55,0x05};//空年月日时分秒周(16进制码表示)
//unsigned char time_buf_pre[8] = {20, 10, 12, 31, 23, 59, 55, 2};//空年月日时分秒周(10进制表示)
unsigned char time_buf_pre[8] = {20, 23, 12, 31, 23, 59, 52, 1};//空年月日时分秒周(10进制表示)
unsigned char time_buf[8];
/*------------------------------------------------
           向DS1302写入一字节数据
------------------------------------------------*/
void Ds1302_Write_Byte(unsigned char addr, unsigned char d)
{

	unsigned char i;
	RST=1;	
	
	//写入目标地址：addr
	addr = addr & 0xFE;     //最低位置零
	for (i = 0; i < 8; i ++) 
	{ 
		if (addr & 0x01) 
		{
			SDA=1;
		}
		else 
		{
			SDA=0;
		}
		SCK=1;
		SCK=0;
		addr = addr >> 1;
	}
	
	//写入数据：d
	for (i = 0; i < 8; i ++) 
	{
		if (d & 0x01) 
		{
			SDA=1;
		}
		else 
		{
			SDA=0;
		}
		SCK=1;
		SCK=0;
		d = d >> 1;
	}
	RST=0;					//停止DS1302总线
}
/*------------------------------------------------
           从DS1302读出一字节数据
------------------------------------------------*/

unsigned char Ds1302_Read_Byte(unsigned char addr) 
{

	unsigned char i;
	unsigned char temp;
	RST=1;	

	//写入目标地址：addr
	addr = addr | 0x01;//最低位置高
	for (i = 0; i < 8; i ++) 
	    {
	     
		if (addr & 0x01) 
		   {
			SDA=1;
			}
		else 
		    {
			SDA=0;
			}
		SCK=1;
		SCK=0;
		addr = addr >> 1;
		}
	
	//输出数据：temp
	for (i = 0; i < 8; i ++) 
	    {
		temp = temp >> 1;
		if (SDA) 
		   {
			temp |= 0x80;
			}
		else 
		   {
			temp &= 0x7F;
			}
		SCK=1;
		SCK=0;
		}
	
	RST=0;	//停止DS1302总线
	return temp;
}

/*------------------------------------------------
           向DS1302写入时钟数据
------------------------------------------------*/
void Ds1302_Write_Time(void) 
{
	Ds1302_Write_Byte(ds1302_control_add,0x00);			//关闭写保护 
	Ds1302_Write_Byte(ds1302_sec_add,0x80);				//暂停 
//	Ds1302_Write_Byte(ds1302_charger_add,0xa9);			//涓流充电 
	Ds1302_Write_Byte(ds1302_year_add,time_buf[1]);		//年 
	Ds1302_Write_Byte(ds1302_month_add,time_buf[2]);	//月 
	Ds1302_Write_Byte(ds1302_date_add,time_buf[3]);		//日 
	Ds1302_Write_Byte(ds1302_hr_add,time_buf[4]);		//时 
	Ds1302_Write_Byte(ds1302_min_add,time_buf[5]);		//分
	Ds1302_Write_Byte(ds1302_sec_add,time_buf[6]);		//秒
	Ds1302_Write_Byte(ds1302_day_add,time_buf[7]);		//周 
	Ds1302_Write_Byte(ds1302_control_add,0x80);			//打开写保护 
}

/*------------------------------------------------
           从DS1302读出时钟数据
------------------------------------------------*/
void Ds1302_Read_Time(void)  
{ 
	time_buf[1]=Ds1302_Read_Byte(ds1302_year_add);		//年 
	time_buf[2]=Ds1302_Read_Byte(ds1302_month_add);		//月 
	time_buf[3]=Ds1302_Read_Byte(ds1302_date_add);		//日 
	time_buf[4]=Ds1302_Read_Byte(ds1302_hr_add);		//时 
	time_buf[5]=Ds1302_Read_Byte(ds1302_min_add);		//分 
	time_buf[6]=(Ds1302_Read_Byte(ds1302_sec_add))&0x7F;//秒，屏蔽秒的第7位，避免超出59
	time_buf[7]=Ds1302_Read_Byte(ds1302_day_add);		//周 

}

/*------------------------------------------------
                DS1302初始化
------------------------------------------------*/
void Ds1302_Init(void)
{
	RST=0;			//RST脚置低
	SCK=0;			//SCK脚置低
    Ds1302_Write_Byte(ds1302_sec_add,0x00);
}


//十进制转BCD
void DEC2BCD(unsigned char *time_buf)
{
    unsigned char i,tmp1,tmp2;
	for(i=0;i<8;i++)
	{
		tmp1=time_buf_pre[i]/10;
		tmp2=time_buf_pre[i]%10;
		time_buf[i]=tmp2+tmp1*16;
	}
}

//十六进制转BCD
void HEX2BCD(unsigned char *time_buf)
{
    unsigned char i,tmp1,tmp2;
	for(i=0;i<8;i++)
	{
		tmp1=time_buf_pre[i]/16;
		tmp2=time_buf_pre[i]%16;
		time_buf[i]=tmp2+tmp1*16;
	}
}

//BCD转十六进制
void BCD2HEX(unsigned char *time_buf)
{
    unsigned char i,tmp1,tmp2;
	for(i=0;i<8;i++)
	{
		tmp1=time_buf[i]>>4;
		tmp2=time_buf[i]&0x0f;
		time_buf[i]=tmp2+tmp1*10;
	}
}