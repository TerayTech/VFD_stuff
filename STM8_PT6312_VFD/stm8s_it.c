/* Includes ------------------------------------------------------------------*/
#include "stm8s_it.h"

extern u32 TimingDelay; 
extern u32 CurSeconds; 

#ifdef _COSMIC_
INTERRUPT_HANDLER(NonHandledInterrupt, 25){}
#endif /*_COSMIC_*/

INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler){}
INTERRUPT_HANDLER(TLI_IRQHandler, 0){}
INTERRUPT_HANDLER(AWU_IRQHandler, 1){}
INTERRUPT_HANDLER(CLK_IRQHandler, 2){}
INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3){}
INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4){}
INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5){}
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6){}
INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7){}
INTERRUPT_HANDLER(SPI_IRQHandler, 10){}
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
    TimingDelay++;    
	if (TimingDelay >= 1000) {
        CurSeconds++;
		TimingDelay = 0;
	}
	TIM1_ClearITPendingBit(TIM1_IT_UPDATE);  
}

INTERRUPT_HANDLER(TIM1_CAP_COM_IRQHandler, 12){}
INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13) { }
INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14) { }
INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17){ }
INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18){ }
INTERRUPT_HANDLER(I2C_IRQHandler, 19){ }
INTERRUPT_HANDLER(ADC1_IRQHandler, 22) { }
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23) { } 
INTERRUPT_HANDLER(EEPROM_EEC_IRQHandler, 24){ }
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
