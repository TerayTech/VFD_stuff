#include"PT6312.h"

int CurSeconds=0; 
int tmp2;

unsigned char DisplayCache[9] = { 0x3F, 0x3f, 0x3f, 0x3f, 0x3f, 0x3F, 0x3f, 0x3f, 0x3f};
unsigned char dict[10]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
void setup()
{
    pinMode(6,OUTPUT);
    pinMode(7,OUTPUT);
    pinMode(8,OUTPUT);
    //pinMode(13,OUTPUT);

    VFD_CLK_H();
    VFD_DIN_H();

    VFD_Init();
    VFD_Display();
    VFD_LED_ON();
}

void loop()
{
    VFD_DATA_2_DisplayCache();
    VFD_Display();
    delay(1000);
    CurSeconds += 1;
}
