#include "mcp3421_V.h"
#include "stm8s.h"
#include "delay.h"
#include "uart.h"
#include "main.h"

u8 V_UpdateFlag;
bool V_OLFlag;

void V_InitMCP3421()
{
    // 0x1C:配置字节, 18bit, 连续转换
    V_Write_MCP3421(0x1C);
}

void V_Test_MCP3421()
{
    u8 byteConfig=0;

    V_I2CStart();
    V_I2CSendByte(0xd1);     // Device Addr + Write (operation)
    V_WaitAck();

    V_I2CReadByte();
    V_SendAck();
    
    V_I2CReadByte();
    V_SendAck();
    
    V_I2CReadByte();
    V_SendAck();

    byteConfig = V_I2CReadByte();
    V_SendNotAck();

    V_I2CStop();

    // 判断是否成功
    byteConfig &= 0x1F;

    // 0x1C:配置字节, 18bit, 连续转换
    if(byteConfig != 0x1C){
        showError(1);
    }
}

void V_Write_MCP3421(unsigned char wr_data)
{
    V_I2CStart();
    V_I2CSendByte(0xd0);  // MCP3421器件地址
    V_WaitAck();
    V_I2CSendByte(wr_data);
    V_WaitAck();
    V_I2CStop();
}

u32 V_Read_MCP3421()
{
    u8 byte1, byte2, byte3, byteConfig;
    u32 temp;

    V_I2CStart();
    V_I2CSendByte(0xd1);
    V_WaitAck();

    byte1 = V_I2CReadByte();
    V_SendAck();

    byte2 = V_I2CReadByte();
    V_SendAck();

    byte3 = V_I2CReadByte();
    V_SendAck();

    byteConfig = V_I2CReadByte();
    V_SendNotAck();

    V_I2CStop();
    
    //18 位, 输出四个字节，即三个数据字节和一个配置字节:
    //MMMMMMD17D16  （第1 个数据字节） - 
    //D15 ~ D8      （第2 个数据字节） - 
    //D7 ~ D0       （第3 个数据字节） - 
    //配置字节。
    // 注意：M 是数据字节重复的MSB（D17）
    
    
    // >= VREF  0x01FFFF= 131071 =  011111111111111111
    // 1 LSB    0x01    =  1     =  000000000000000001
    // 0=0=                         000000000000000000
    // -1 LSB   0x03FFFF= 262143 =  111111111111111111(17个1）
    // < -VREF  0x020000= 131072 =  100000000000000000
    
    /*    
    UART1_SendByte(0x0D);
    UART1_SendByte(byte1);
    UART1_SendByte(byte2);
    UART1_SendByte(byte3);*/
    V_OLFlag = 0;
    
    if(byteConfig & 0x80) // 新的一次尚未转换成功，返回0
    {
        V_UpdateFlag = 0;
        temp = 0;
    }
    else
    {
        V_UpdateFlag = 1;
        
        // 如果忽略前7个bit，使用&0x01
        // 如果忽略前6个bit，使用&0x03, 此时需要注意负压的补码计算
        
        
        if(byte1 == 0xFF && byte2== 0xFF && byte3 == 0xFF)
            return 0; //
        
        if(byte1 & 0x02) // 取MSB
        {
            V_OLFlag = 1;
            // 负电压，直接输出0
            return 0;
        }
        
        temp = (byte1 & 0x01)<<8; // 正电压直接忽略前7个bit
        temp |= byte2;
        temp <<= 8;
        temp |= byte3;
    }

    return temp;
}


////////////////////////////////////////////////////////////////////////////
// IIC基础函数区

//启动I2C总线,即发送I2C起始条件
void V_I2CStart(void)
{
    V_SDA_H();      /* 发送起始条件的数据信号*/
    V_SCL_H();      /* 起始条件建立时间大于4.7us,延时*/
    IIC_Delay_5us();
    V_SDA_L();      /* 发送起始信号*/
    IIC_Delay_5us();  /* 起始条件锁定时间大于4μs*/
    V_SCL_L();      /* 钳住I2C总线，准备发送或接收数据 */
}

//结束I2C总线,即发送I2C结束条件
void V_I2CStop(void)
{
    V_SDA_L();       /*发送结束条件的数据信号*/
    V_SCL_H();
    IIC_Delay_5us();   /*结束条件建立时间大于4μs*/
    V_SDA_H();       /*发送I2C总线结束信号*/
    IIC_Delay_5us();
}

void V_WaitAck(void) //等待I2C应答信号
{
    u8 i = 2,j=0;
    
    V_SDA_H();
    V_SCL_L();
    IIC_Delay_5us();
    V_SCL_H();
    while(i--)
    {
        j = V_SDA_Read();
        if(j==0)	goto loop;
    }
    
    loop:
        V_SCL_L();
        return;
}

// 发出应答信号
void V_SendAck(void)
{
    V_SDA_L();
    IIC_Delay_5us();

    V_SCL_H();
    IIC_Delay_5us();
    V_SCL_L();
}

// 发出非应答信号
void V_SendNotAck(void)
{
    V_SDA_H();
    IIC_Delay_5us();

    V_SCL_H();
    IIC_Delay_5us();
    V_SCL_L();
}

void V_I2CSendByte(u8 ch)
{
    unsigned char i=8;
    while(i--)
    {
        V_SCL_L();
        IIC_Delay_5us();  // 轻微延时

        if(ch&0x80) /*发送位*/
        {
            V_SDA_H();
        }
        else
        {
            V_SDA_L();
        }

        ch<<=1;
        V_SCL_H();    //置时钟线为高，通知被控器开始接收数据位
        IIC_Delay_5us();  //保证时钟高电平周期大于4us
    }
    V_SCL_L();
}

u8 V_I2CReadByte()
{
    u8 i, j, revDAT = 0;

    V_SDA_H();

    for (i=0; i<8; i++)
    {
        revDAT <<= 1;

        V_SCL_H();

        j = V_SDA_Read();
        
        if (j)
        {
            revDAT |= 0x01;
        }
        else
        {
            revDAT &= 0xfe;
        }

        V_SCL_L();
    }

    return revDAT;
}