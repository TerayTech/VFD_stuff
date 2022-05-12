#include "stm8s.h"
#include "delay.h"

// 需较精确的延时，IIC使用
void IIC_Delay_5us()
{   
    u8 i;
	nop();
	nop();
	nop();
    
    for(i=300;i>0;i--);
}
void Delay_5us()
{   
    u8 i;
	nop();
	nop();
	nop();
    
    for(i=300;i>0;i--);
}


void delay_ms2(u16 x)
{
    u16 i,j;
    
    for(i=x;i>0;i--) 
      
      for(j=2650;j>0;j--)
      {
           nop();   
      }

}
/*
void delay_ms(u16 x) //us
{
    u16 i,j;
    
    for(i=x;i>0;i--) 
      
      for(j=260;j>0;j--)
      {
           nop();   
      }
}
*/
void delay_ms3(u16 x) //us
{
    u16 i,j;
    
    for(i=x;i>0;i--) 
      
      for(j=100;j>0;j--)
      {
           nop();   
      }
}
void delay_us(u16 x)
{
    u16 i,j;
    
    for(i=x;i>0;i--) 
      
      for(j=2;j>0;j--)
      {
           nop();   
      }

}