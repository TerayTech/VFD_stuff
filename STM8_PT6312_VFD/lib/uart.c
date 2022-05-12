#include "uart.h"
#include "stm8s.h"
#include <stdarg.h>

void Uart1_Init(void)
{
    // 串口复位
    UART1_DeInit();
    UART1_Init((u32)115200,             // 波特率
               UART1_WORDLENGTH_8D,     // 8bit
               UART1_STOPBITS_1,        // 1位截止位
               UART1_PARITY_NO,         // 无校验位
               UART1_SYNCMODE_CLOCK_DISABLE,    // 同步模式和串口时钟关闭
               UART1_MODE_TXRX_ENABLE           // 使能接收和发送
               );
    // 注意库版本，本程序需要2.1库
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);  // 使能接收中断
    //UART1_ITConfig(UART1_IT_RXNE,ENABLE  ); // 老的库版本用这个
    
    UART1_Cmd(ENABLE);  // 使能UART1外设
}


void UART1_SendByte(u8 data)
{
    UART1_SendData8((unsigned char)data);
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
}

void UART1_SendString(u8* Data,u16 len)
{
    u16 i=0;
    for(;i<len;i++)
        UART1_SendByte(Data[i]);
  
}

u8 UART1_ReceiveByte(void)
{
     u8 USART1_RX_BUF; 
     while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
     USART1_RX_BUF=UART1_ReceiveData8();
     return  USART1_RX_BUF;
    
}

/************************************************************************
 * 函数名：USART1_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口1，即USART1
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART1_printf( "\r\n this is a demo \r\n" );
 *            		 USART1_printf(  "\r\n %d \r\n", i );
 *            		 USART1_printf(  "\r\n %s \r\n", j );
 ***************************************************************************/
void USART1_printf( uint8_t *Data,...)
{
  const char *s;
  int d;   
  char buf[16];
  va_list ap;
  va_start(ap, Data);

  while ( *Data != 0)     // 判断是否到达字符串结束符
  {				                          
      if ( *Data == 0x5c )  //'\'
      {									  
          switch ( *++Data )
          {
              case 'r':							          //回车符
                  UART1_SendData8(0x0d);
                  Data ++;
                  break;

              case 'n':							          //换行符
                  UART1_SendData8(0x0a);	
                  Data ++;
                  break;
              
              default:
                  Data ++;
                  break;
          }			 
      }
      else if ( *Data == '%')
      {					//
	switch ( *++Data )
	  {				
	    case 's':						//字符串
		s = va_arg(ap, const char *);
          for ( ; *s; s++) 
	  {
	    UART1_SendData8(*s);
	    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
          }
	  Data++;
          break;

          case 'd':	//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
	{
	  UART1_SendData8(*s);
	  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
        }
	Data++;
        break;
	default:
	Data++;
	break;
	}		 
     } // end of else if 
    else UART1_SendData8(*Data++);
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
  }
}