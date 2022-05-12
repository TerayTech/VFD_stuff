#include "uart.h"
#include "stm8s.h"
#include <stdarg.h>

void Uart1_Init(void)
{
    // ���ڸ�λ
    UART1_DeInit();
    UART1_Init((u32)115200,             // ������
               UART1_WORDLENGTH_8D,     // 8bit
               UART1_STOPBITS_1,        // 1λ��ֹλ
               UART1_PARITY_NO,         // ��У��λ
               UART1_SYNCMODE_CLOCK_DISABLE,    // ͬ��ģʽ�ʹ���ʱ�ӹر�
               UART1_MODE_TXRX_ENABLE           // ʹ�ܽ��պͷ���
               );
    // ע���汾����������Ҫ2.1��
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);  // ʹ�ܽ����ж�
    //UART1_ITConfig(UART1_IT_RXNE,ENABLE  ); // �ϵĿ�汾�����
    
    UART1_Cmd(ENABLE);  // ʹ��UART1����
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
 * ��������USART1_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���1����USART1
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��USART1_printf( "\r\n this is a demo \r\n" );
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

  while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
  {				                          
      if ( *Data == 0x5c )  //'\'
      {									  
          switch ( *++Data )
          {
              case 'r':							          //�س���
                  UART1_SendData8(0x0d);
                  Data ++;
                  break;

              case 'n':							          //���з�
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
	    case 's':						//�ַ���
		s = va_arg(ap, const char *);
          for ( ; *s; s++) 
	  {
	    UART1_SendData8(*s);
	    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
          }
	  Data++;
          break;

          case 'd':	//ʮ����
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