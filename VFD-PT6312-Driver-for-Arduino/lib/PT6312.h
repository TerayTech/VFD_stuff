#ifndef __PT6312_H
#define __PT6312_H

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

void Write_PT6312(unsigned char dat);
void send_cs();
void VFD_Init();
void VFD_Display_Test();
void VFD_LED_ON();
void VFD_LED_OFF();
void VFD_Display();
void VFD_DATA_2_DisplayCache();


#endif