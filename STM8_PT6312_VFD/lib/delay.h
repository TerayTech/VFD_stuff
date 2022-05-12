#ifndef __DELAY_H
#define __DELAY_H

// 需较精确的延时，IIC使用
void IIC_Delay_5us();

void delay_us(u16 x);
void delay_ms(u16 x);
void delay_ms2(u16 x);
void delay_ms3(u16 x);

#endif