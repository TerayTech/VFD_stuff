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
    // 0x1C:�����ֽ�, 18bit, ����ת��,pga=1, 0x1F, pga=8
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

    // �ж��Ƿ�ɹ�
    byteConfig &= 0x1F;

    if(byteConfig != 0x1F) // 0x1C:�����ֽ�
    {        
        showError(1+0x64);
    }
}

void A_Write_MCP3421(unsigned char wr_data)
{
    A_I2CStart();
    A_I2CSendByte(0xd0);  // MCP3421������ַ
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
    
    if(byteConfig & 0x80) // �µ�һ����δת���ɹ�������0
    {
        A_UpdateFlag = 0;
        temp = 0;
    }
    else
    {
        if(byte1 & 0x02) // ȡ��һbit��MSB
        {
            //A_OLFlag = 1;
            // ��������ֱ�����0
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
// IIC����������

//����I2C����,������I2C��ʼ����
void A_I2CStart(void)
{
    A_SDA_H();      /* ������ʼ�����������ź�*/
    A_SCL_H();      /* ��ʼ��������ʱ�����4.7us,��ʱ*/
    IIC_Delay_5us();
    A_SDA_L();      /* ������ʼ�ź�*/
    IIC_Delay_5us();  /* ��ʼ��������ʱ�����4��s*/
    A_SCL_L();      /* ǯסI2C���ߣ�׼�����ͻ�������� */
}

//����I2C����,������I2C��������
void A_I2CStop(void)
{
    A_SDA_L();       /*���ͽ��������������ź�*/
    A_SCL_H();
    IIC_Delay_5us();   /*������������ʱ�����4��s*/
    A_SDA_H();       /*����I2C���߽����ź�*/
    IIC_Delay_5us();
}

void A_WaitAck(void) //�ȴ�I2CӦ���ź�
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

// ����Ӧ���ź�
void A_SendAck(void)
{
    A_SDA_L();
    IIC_Delay_5us();

    A_SCL_H();
    IIC_Delay_5us();
    A_SCL_L();
}

// ������Ӧ���ź�
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
        IIC_Delay_5us();  // ��΢��ʱ

        if(ch&0x80) /*����λ*/
        {
            A_SDA_H();
        }
        else
        {
            A_SDA_L();
        }

        ch<<=1;
        A_SCL_H();    //��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ
        IIC_Delay_5us();  //��֤ʱ�Ӹߵ�ƽ���ڴ���4us
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