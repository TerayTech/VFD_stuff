#include "stm8s.h"
#include "PT6312.h"

u8 dict[]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
u8 DisplayCache[9] = { 0x3F, 0x3f, 0x3f, 0x3f, 0x3f, 0x3F, 0x3f, 0x3f, 0x3f};

u32 TimingDelay=0; 
u32 CurSeconds=0; 

void InitGPIO()
{
    // DIN--PD3 
    // STB--PC7
    // CLK--PD2 
    // VFD ON == PC6   SHEN 
    GPIO_Init(GPIOC, GPIO_PIN_6|GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOD, GPIO_PIN_2|GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);
}

void Tim1_Init(void)
{
	TIM1_TimeBaseInit(15, TIM1_COUNTERMODE_UP, 999, 0);
	TIM1_SetCounter(0);
	TIM1_ARRPreloadConfig(DISABLE);
	TIM1_ITConfig(TIM1_IT_UPDATE , ENABLE);
	TIM1_Cmd(ENABLE);
}

void main(void)
{
    u8 tmp;
    u32 tmp2;
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); //内部时钟分频1 
    
    InitGPIO();
    
    VFD_Init();
    
    VFD_Display();
    
    // VFD_ON(); // 默认电平就是开启的    
    VFD_LED_ON();
    
    // 使用定时器走秒
    Tim1_Init();
    enableInterrupts();
    
    while (1)
    {
        while(tmp2 == CurSeconds);
        
        // 更新秒数到VFD
        tmp2 = CurSeconds;
        
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
        
        VFD_Display();
    }
  
}

#ifdef USE_FULL_ASSERT

void assert_failed(u8* file, u32 line)
{ 
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
