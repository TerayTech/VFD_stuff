// 电压表使用
#ifndef __MCP3421_V_H
#define __MCP3421_V_H
#include "stm8s.h"

// sda -- PA2
#define V_IIC_SCLPort GPIOB
#define V_IIC_SCL GPIO_PIN_4

#define V_IIC_SDAPort GPIOB
#define V_IIC_SDA GPIO_PIN_5

#define V_SCL_H() GPIO_WriteHigh(V_IIC_SCLPort, V_IIC_SCL);
#define V_SCL_L() GPIO_WriteLow(V_IIC_SCLPort, V_IIC_SCL);
#define V_SDA_H() GPIO_WriteHigh(V_IIC_SDAPort, V_IIC_SDA);
#define V_SDA_L() GPIO_WriteLow(V_IIC_SDAPort, V_IIC_SDA);
#define V_SDA_Read() GPIO_ReadInputPin(V_IIC_SDAPort, V_IIC_SDA);

void V_InitMCP3421();
void V_Test_MCP3421();// 
void V_Write_MCP3421(unsigned char wr_data) ;
u32 V_Read_MCP3421();

//启动I2C总线,即发送I2C起始条件
void V_I2CStart(void);

//结束I2C总线,即发送I2C结束条件
void V_I2CStop(void);

//等待I2C应答信号;
void V_WaitAck(void); 

// 发出应答信号
void V_SendAck(void) ;

// 发出非应答信号
void V_SendNotAck(void);

// 发送一个字节数据
void V_I2CSendByte(u8 ch)  ;

// 读取一个字节数据
u8 V_I2CReadByte();

#endif