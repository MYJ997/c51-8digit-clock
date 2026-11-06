// 8位数码管显示 DS1302时钟：HH-MM-SS
#include<reg52.h>
#include<intrins.h>

extern unsigned char time_buf[8];
void Ds1302_Init(void);
void Ds1302_Read_Time(void);
void BCD2HEX(unsigned char *buf);
void DEC2BCD(unsigned char *buf);
void Ds1302_Write_Time(void);
void Ds1302_Write_Byte(unsigned char addr,unsigned char dat);

#define Seg8Bit		P0
#define Seg8Seg		P1

#define LEDLen 8

unsigned char LEDBuf[LEDLen];
code unsigned char LEDMAP[] = {
  0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
  0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71
};

void Delay(unsigned char CNT)
{
    unsigned char i;
    while (CNT-- !=0)
        for (i=200; i !=0; i--);
}

void DisplayLED()
{
    unsigned char i, j;
    unsigned char Pos;
    unsigned char tmp;

    Pos = 0x80;
    for (i = 0; i < LEDLen; i++)
    {
        Seg8Bit = 0;
        tmp = LEDBuf[i];
        Seg8Seg = tmp;
        Seg8Bit = Pos;
        Delay(1);
        Pos >>= 1;
    }
    Seg8Bit = 0;
}

void ShowTime(void)
{
    unsigned char h,m,s;

    Ds1302_Read_Time();
    BCD2HEX(time_buf);

    h = time_buf[4];
    m = time_buf[5];
    s = time_buf[6];

    LEDBuf[7] = LEDMAP[h/10];
    LEDBuf[6] = LEDMAP[h%10];
    LEDBuf[5] = 0x40;
    LEDBuf[4] = LEDMAP[m/10];
    LEDBuf[3] = LEDMAP[m%10];
    LEDBuf[2] = 0x40;
    LEDBuf[1] = LEDMAP[s/10];
    LEDBuf[0] = LEDMAP[s%10];
}

void main()
{
    unsigned char j;

    Ds1302_Init();

    // ===================== 唯一正确的写入方式 =====================
    Ds1302_Write_Byte(0x8E, 0x00);   // 关闭写保护

    // 直接写入正确的时间，不碰停止位
    Ds1302_Write_Byte(0x80, 0x00);   // 秒：00，启动时钟
    Ds1302_Write_Byte(0x82, 0x00);   // 分：00
    Ds1302_Write_Byte(0x84, 0x10);   // 时：10

    Ds1302_Write_Byte(0x8E, 0x80);   // 开启写保护
    // ==============================================================

    while(1) {
        ShowTime();
        for(j=0; j<50; j++)
            DisplayLED();
    }
}