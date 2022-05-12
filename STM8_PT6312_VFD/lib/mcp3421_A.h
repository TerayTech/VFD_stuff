// 电流表使用
#ifndef __MCP3421_A_H
#define __MCP3421_A_H
#include "stm8s.h"

// sda -- PA2
#define A_IIC_Port GPIOC
#define A_IIC_SCL GPIO_PIN_4
#define A_IIC_SDA GPIO_PIN_3

#define A_SDA_H() GPIO_WriteHigh(A_IIC_Port, A_IIC_SDA);
#define A_SDA_L() GPIO_WriteLow(A_IIC_Port, A_IIC_SDA);
#define A_SCL_H() GPIO_WriteHigh(A_IIC_Port, A_IIC_SCL);
#define A_SCL_L() GPIO_WriteLow(A_IIC_Port, A_IIC_SCL);
#define A_SDA_Read() GPIO_ReadInputPin(A_IIC_Port, A_IIC_SDA);


void A_InitMCP3421();
void A_Test_MCP3421();// 
void A_Write_MCP3421(unsigned char wr_data) ;
u32 A_Read_MCP3421();



//启动I2C总线,即发送I2C起始条件
void A_I2CStart(void);

//结束I2C总线,即发送I2C结束条件
void A_I2CStop(void);

//等待I2C应答信号;
void A_WaitAck(void); 

// 发出应答信号
void A_SendAck(void) ;

// 发出非应答信号
void A_SendNotAck(void);

// 发送一个字节数据
void A_I2CSendByte(u8 ch)  ;

// 读取一个字节数据
u8 A_I2CReadByte();

#endif