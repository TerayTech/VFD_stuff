#include "PT6312.h"
#include "stm8s.h"

// DIN--pa1 
// STB--pa2
// clk--pd3 
// VFD ON == PD2
#define VFD_CLK_H() GPIO_WriteHigh(GPIOD, GPIO_PIN_2);
#define VFD_CLK_L() GPIO_WriteLow(GPIOD, GPIO_PIN_2);

#define VFD_DIN_H() GPIO_WriteHigh(GPIOD, GPIO_PIN_3);
#define VFD_DIN_L() GPIO_WriteLow(GPIOD, GPIO_PIN_3);

#define VFD_STB_H() GPIO_WriteHigh(GPIOC, GPIO_PIN_7);
#define VFD_STB_L() GPIO_WriteLow(GPIOC, GPIO_PIN_7);

extern u8 DisplayCache[9];
extern u8 vfdmap[];

u8 vfdbuf[]={0,0,0,0,0,0,0};

#define MODE 0x05  // VFD Dan set ( 9 digit*13 segment) 
#define LIGHT 0x05 // VFD Brightness settings (0x00-0x07) 
#define ON 0x08    // Open Display 
#define OFF 0x00   // Off Display

void Write_PT6312 (u8 dat)
{
    u8 i;
    for(i=0;i<8;i++)
    {
        VFD_CLK_L();
        if(dat&0x01) {
            VFD_DIN_H();
        } else {            
            VFD_DIN_L();
        }
        dat>>=1;
        VFD_CLK_H();
    }
}
void send_cs(void)
{
    VFD_STB_H();
    VFD_STB_L();
}

void VFD_Init(void)
{
    u8 i;    
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
}
void VFD_Display_Test(void)
{
    u8 i;
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

void VFD_LED_ON()
{
    // 打开LED显示
    send_cs();
    Write_PT6312(0x41); // 设定LED
    Write_PT6312(0x0C); // led1&LED2
    VFD_STB_H();    
}
void VFD_LED_OFF()
{
    // 关闭LED显示
    send_cs();
    Write_PT6312(0x41); // 设定LED
    Write_PT6312(0x0F); // led1&LED2
    VFD_STB_H();    
}
void VFD_Display()
{
    u8 i;
    u8* p = DisplayCache;
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
}