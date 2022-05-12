#include "mcp3421_A.h"
#include "stm8s.h"
#include "delay.h"
#include "uart.h"
#include "main.h"

u8 A_UpdateFlag;
bool A_OLFlag;

//#define aConfig 0x1F

//const u8 aConfig= 0x1F;

void A_InitMCP3421()
{
    // 0x1C:配置字节, 18bit, 连续转换,pga=1, 0x1F, pga=8
    A_Write_MCP3421(0x1F);// 
}

void A_Test_MCP3421()
{
    u8 byteConfig;

    A_I2CStart();
    A_I2CSendByte(0xD1);     // Device Addr + Write (operation)
    A_WaitAck();

    A_I2CReadByte();
    A_SendAck();

    A_I2CReadByte();
    A_SendAck();

    A_I2CReadByte();
    A_SendAck();

    byteConfig = A_I2CReadByte();
    A_SendNotAck();

    A_I2CStop();

    // 判断是否成功
    byteConfig &= 0x1F;

    if(byteConfig != 0x1F) // 0x1C:配置字节
    {        
        showError(1+0x64);
    }
}

void A_Write_MCP3421(unsigned char wr_data)
{
    A_I2CStart();
    A_I2CSendByte(0xd0);  // MCP3421器件地址
    A_WaitAck();
    A_I2CSendByte(wr_data);
    A_WaitAck();
    A_I2CStop();
}

u32 A_Read_MCP3421()
{
    u8 byte1, byte2, byte3, byteConfig;
    u32 temp;

    A_I2CStart();
    A_I2CSendByte(0xd1);
    A_WaitAck();

    byte1 = A_I2CReadByte();
    A_SendAck();

    byte2 = A_I2CReadByte();
    A_SendAck();

    byte3 = A_I2CReadByte();
    A_SendAck();

    byteConfig = A_I2CReadByte();
    A_SendNotAck();

    A_I2CStop();

    /*
    UART1_SendByte(0x0A);
    UART1_SendByte(byte1);
    UART1_SendByte(byte2);
    UART1_SendByte(byte3);*/
    A_OLFlag = 0;  
    
    if(byteConfig & 0x80) // 新的一次尚未转换成功，返回0
    {
        A_UpdateFlag = 0;
        temp = 0;
    }
    else
    {
        if(byte1 & 0x02) // 取第一bit即MSB
        {
            //A_OLFlag = 1;
            // 负电流，直接输出0
            return 0;
        }      
        A_UpdateFlag = 1;
        
        temp = (byte1 & 0x01) << 8;
        temp |= byte2;
        temp <<= 8;
        temp |= byte3;
    }
    

    return temp;
}


////////////////////////////////////////////////////////////////////////////
// IIC基础函数区

//启动I2C总线,即发送I2C起始条件
void A_I2CStart(void)
{
    A_SDA_H();      /* 发送起始条件的数据信号*/
    A_SCL_H();      /* 起始条件建立时间大于4.7us,延时*/
    IIC_Delay_5us();
    A_SDA_L();      /* 发送起始信号*/
    IIC_Delay_5us();  /* 起始条件锁定时间大于4μs*/
    A_SCL_L();      /* 钳住I2C总线，准备发送或接收数据 */
}

//结束I2C总线,即发送I2C结束条件
void A_I2CStop(void)
{
    A_SDA_L();       /*发送结束条件的数据信号*/
    A_SCL_H();
    IIC_Delay_5us();   /*结束条件建立时间大于4μs*/
    A_SDA_H();       /*发送I2C总线结束信号*/
    IIC_Delay_5us();
}

void A_WaitAck(void) //等待I2C应答信号
{
    u8 i = 2,j=0;
    
    A_SDA_H();
    A_SCL_L();
    IIC_Delay_5us();
    A_SCL_H();
    while(i--)
    {
        j = A_SDA_Read();
        if(j==0)	goto loop;
    }
    loop:
        A_SCL_L();
        return;
}

// 发出应答信号
void A_SendAck(void)
{
    A_SDA_L();
    IIC_Delay_5us();

    A_SCL_H();
    IIC_Delay_5us();
    A_SCL_L();
}

// 发出非应答信号
void A_SendNotAck(void)
{
    A_SDA_H();
    IIC_Delay_5us();

    A_SCL_H();
    IIC_Delay_5us();
    A_SCL_L();
}

void A_I2CSendByte(u8 ch)
{
    u8 i=8;
    while(i--)
    {
        A_SCL_L();
        IIC_Delay_5us();  // 轻微延时

        if(ch&0x80) /*发送位*/
        {
            A_SDA_H();
        }
        else
        {
            A_SDA_L();
        }

        ch<<=1;
        A_SCL_H();    //置时钟线为高，通知被控器开始接收数据位
        IIC_Delay_5us();  //保证时钟高电平周期大于4us
    }
    A_SCL_L();
}

u8 A_I2CReadByte()
{
    u8 i,j;
    u8 revDAT = 0;

    A_SDA_H();

    for (i=0; i<8; i++)
    {
        revDAT <<= 1;

        A_SCL_H();

        j = A_SDA_Read();
        
        if (j)
            revDAT |= 0x01;
        else
            revDAT &= 0xfe;

        A_SCL_L();
    }

    return revDAT;
}