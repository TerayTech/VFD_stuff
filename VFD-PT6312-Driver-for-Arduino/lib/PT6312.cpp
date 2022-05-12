/******************* (C) COPYRIGHT 2019 TerayTechEnterprise *****START OF FILE****/
#include "Arduino.h"

#define VFD_CLK_H() digitalWrite(6,LOW);
#define VFD_CLK_L() digitalWrite(6, HIGH);

#define VFD_DIN_H() digitalWrite(7, HIGH);
#define VFD_DIN_L() digitalWrite(7, LOW);

#define VFD_STB_H() digitalWrite(8, HIGH);
#define VFD_STB_L() digitalWrite(8, LOW);

extern int CurSeconds;

extern unsigned char vfdmap[9];

//unsigned char vfdbuf[7]={0,0,0,0,0,0,0};

extern unsigned char dict[];
extern unsigned char DisplayCache[9];

#define MODE 0x05  // VFD 显示模式设定 ( 9 digit*13 segment) 
#define LIGHT 0x05 // VFD 亮度设定 (0x00-0x07) 
#define ON 0x08    // 打开显示
#define OFF 0x00   // 关闭显示


/******************************函数定义部分**************************/

void Write_PT6312(unsigned char dat)
{
    int i;
    for(i=0;i<8;i++)
    {
        VFD_CLK_H();
        if(dat&0x01) {
          VFD_DIN_H();
        } else {            
            VFD_DIN_L();
        }
        dat>>=1;
        VFD_CLK_L();
    }
    delayMicroseconds(6);
}

void send_cs(void)
{
    VFD_STB_H();
    VFD_STB_L();
}

//VFD初始化
void VFD_Init(void)
{
    int i;    
    send_cs();
    Write_PT6312(MODE); // 段位设置
    send_cs();
    Write_PT6312(0x40); // 设定数据
    send_cs(); 
    Write_PT6312(0xc0); // Set Address
    for (i=0;i<9;i++)
    {
        Write_PT6312(0x00);  // Transfer display data low byte 
        Write_PT6312(0x00);  // High byte data transmission display
    }
    send_cs(); 
    Write_PT6312(0x80|ON|LIGHT); // Display Control
    VFD_STB_H();
    VFD_DIN_H();
}

//显示测试
void VFD_Display_Test(void)
{
    int i;
    send_cs();
    Write_PT6312(0xc0);  // Set Address
    for (i=0;i<9;i++)
    {
        Write_PT6312(vfdmap[i]); // Transfer display data low byte 
        Write_PT6312(0x00);        // High byte data transmission display
    }
    send_cs(); 
    Write_PT6312(0x80|ON|LIGHT); // Display Control
    VFD_STB_H();
    
}

void VFD_LED_ON(void)
{
    send_cs();
    Write_PT6312(0x41); // 设定LED
    Write_PT6312(0x0C); // led1&LED2
    VFD_STB_H();    
}

void VFD_LED_OFF(void)
{
    send_cs();
    Write_PT6312(0x41); // 设定LED
    Write_PT6312(0x0F); // led1&LED2
    VFD_STB_H();    
}

void VFD_Display(void)
{
    int i;
    unsigned char* p = DisplayCache;
    send_cs();
    Write_PT6312(0xc0);
    for (i=0;i<9;i++)
    {
        Write_PT6312(*p++);
        Write_PT6312(0x00);
    }
    send_cs(); 
    Write_PT6312(0x80|ON|LIGHT);
    VFD_STB_H();
    VFD_DIN_H();
}

void VFD_DATA_2_DisplayCache(void)
{
    unsigned char tmp;
    tmp = CurSeconds /1000000;
    DisplayCache[2] = dict[tmp];
    tmp = CurSeconds %1000000/100000;
    DisplayCache[3] = dict[tmp];
    tmp = CurSeconds %100000/10000;
    DisplayCache[4] = dict[tmp];
    tmp = CurSeconds %10000/1000;
    DisplayCache[5] = dict[tmp];
    tmp = CurSeconds %1000/100;
    DisplayCache[6] = dict[tmp];
    tmp = CurSeconds %100/10;
    DisplayCache[7] = dict[tmp];
    tmp = CurSeconds %10;
    DisplayCache[8] = dict[tmp];
}

/**********************函数定义结束*************************/