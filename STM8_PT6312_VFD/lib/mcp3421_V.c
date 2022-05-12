#include "mcp3421_V.h"
#include "stm8s.h"
#include "delay.h"
#include "uart.h"
#include "main.h"

u8 V_UpdateFlag;
bool V_OLFlag;

void V_InitMCP3421()
{
    // 0x1C:�����ֽ�, 18bit, ����ת��
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

    // �ж��Ƿ�ɹ�
    byteConfig &= 0x1F;

    // 0x1C:�����ֽ�, 18bit, ����ת��
    if(byteConfig != 0x1C){
        showError(1);
    }
}

void V_Write_MCP3421(unsigned char wr_data)
{
    V_I2CStart();
    V_I2CSendByte(0xd0);  // MCP3421������ַ
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
    
    //18 λ, ����ĸ��ֽڣ������������ֽں�һ�������ֽ�:
    //MMMMMMD17D16  ����1 �������ֽڣ� - 
    //D15 ~ D8      ����2 �������ֽڣ� - 
    //D7 ~ D0       ����3 �������ֽڣ� - 
    //�����ֽڡ�
    // ע�⣺M �������ֽ��ظ���MSB��D17��
    
    
    // >= VREF  0x01FFFF= 131071 =  011111111111111111
    // 1 LSB    0x01    =  1     =  000000000000000001
    // 0=0=                         000000000000000000
    // -1 LSB   0x03FFFF= 262143 =  111111111111111111(17��1��
    // < -VREF  0x020000= 131072 =  100000000000000000
    
    /*    
    UART1_SendByte(0x0D);
    UART1_SendByte(byte1);
    UART1_SendByte(byte2);
    UART1_SendByte(byte3);*/
    V_OLFlag = 0;
    
    if(byteConfig & 0x80) // �µ�һ����δת���ɹ�������0
    {
        V_UpdateFlag = 0;
        temp = 0;
    }
    else
    {
        V_UpdateFlag = 1;
        
        // �������ǰ7��bit��ʹ��&0x01
        // �������ǰ6��bit��ʹ��&0x03, ��ʱ��Ҫע�⸺ѹ�Ĳ������
        
        
        if(byte1 == 0xFF && byte2== 0xFF && byte3 == 0xFF)
            return 0; //
        
        if(byte1 & 0x02) // ȡMSB
        {
            V_OLFlag = 1;
            // ����ѹ��ֱ�����0
            return 0;
        }
        
        temp = (byte1 & 0x01)<<8; // ����ѹֱ�Ӻ���ǰ7��bit
        temp |= byte2;
        temp <<= 8;
        temp |= byte3;
    }

    return temp;
}


////////////////////////////////////////////////////////////////////////////
// IIC����������

//����I2C����,������I2C��ʼ����
void V_I2CStart(void)
{
    V_SDA_H();      /* ������ʼ�����������ź�*/
    V_SCL_H();      /* ��ʼ��������ʱ�����4.7us,��ʱ*/
    IIC_Delay_5us();
    V_SDA_L();      /* ������ʼ�ź�*/
    IIC_Delay_5us();  /* ��ʼ��������ʱ�����4��s*/
    V_SCL_L();      /* ǯסI2C���ߣ�׼�����ͻ�������� */
}

//����I2C����,������I2C��������
void V_I2CStop(void)
{
    V_SDA_L();       /*���ͽ��������������ź�*/
    V_SCL_H();
    IIC_Delay_5us();   /*������������ʱ�����4��s*/
    V_SDA_H();       /*����I2C���߽����ź�*/
    IIC_Delay_5us();
}

void V_WaitAck(void) //�ȴ�I2CӦ���ź�
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

// ����Ӧ���ź�
void V_SendAck(void)
{
    V_SDA_L();
    IIC_Delay_5us();

    V_SCL_H();
    IIC_Delay_5us();
    V_SCL_L();
}

// ������Ӧ���ź�
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
        IIC_Delay_5us();  // ��΢��ʱ

        if(ch&0x80) /*����λ*/
        {
            V_SDA_H();
        }
        else
        {
            V_SDA_L();
        }

        ch<<=1;
        V_SCL_H();    //��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ
        IIC_Delay_5us();  //��֤ʱ�Ӹߵ�ƽ���ڴ���4us
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