/******************* (C) COPYRIGHT 2019 TerayTechEnterprise *****START OF FILE****/


//输出端口定义
// DIN--2
// STB--3
// clk--4
// VFD ON == 5
int VFD_DIN = 2;
int VFD_STB = 3;
int VFD_CLK = 4;
int VFD_EN  = 5;

#define VFD_CLK_H() digitalWrite(VFD_CLK,HIGH);
#define VFD_CLK_L() digitalWrite(VFD_CLK, LOW);

#define VFD_DIN_H() digitalWrite(VFD_DIN, HIGH);
#define VFD_DIN_L() digitalWrite(VFD_DIN, LOW);

#define VFD_STB_H() digitalWrite(VFD_STB, HIGH);
#define VFD_STB_L() digitalWrite(VFD_STB, LOW);

//显存(如果显示出问题检查这里)
unsigned char vfdmap[9];

u8 vfdbuf[]={0,0,0,0,0,0,0};

#define MODE 0x05  // VFD 显示模式设定 ( 9 digit*13 segment) 
#define LIGHT 0x05 // VFD 亮度设定 (0x00-0x07) 
#define ON 0x08    // 打开显示
#define OFF 0x00   // 关闭显示



unsigned char dict[]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
unsigned char DisplayCache[9] = { 0x3F, 0x3f, 0x3f, 0x3f, 0x3f, 0x3F, 0x3f, 0x3f, 0x3f};

int TimingDelay=0; 
int CurSeconds=0; 





void setup() {

    //PIN输出模式定义
    pinMode(VFD_DIN,OUTPUT);
    pinMode(VFD_STB,OUTPUT);
    pinMode(VFD_CLK,OUTPUT);


}



void loop() {

    unsigned char tmp;
    int tmp2;

    VFD_Init();
    VFD_Display();
    VFD_LED_ON();

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












/******************************函数定义部分**************************/

//数据帧写操作
void Write_PT6312(unsigned char dat)
{
    int i;
    for(i=0;i<8;i++)
    {
        digitalWrite(VFD_CLK,HIGH);
        if(dat&0x01) {
          digitalWrite(VFD_DIN,HIGH);
        } else {            
            digitalWrite(VFD_DIN,LOW);
        }
        dat>>=1;
        digitalWrite(VFD_CLK,LOW);
    }
}

void send_cs()
{
    VFD_STB_H();
    VFD_STB_L();
}

//VFD初始化
void VFD_Init()
{
    int i;    
    send_cs();
    Write_PT6312(MODE); // 段位设置
    send_cs();
    Write_PT6312(0x40); // 设定数据
    send_cs(); 
    Write_PT6312(0xc0); // Set Address
    for (i=0;i<9;i++)
    {
        Write_PT6312(0x00);  // Transfer display data low byte 
        Write_PT6312(0x00);  // High byte data transmission display
    }
    send_cs(); 
    Write_PT6312(0x80|ON|LIGHT); // Display Control
    VFD_STB_H();
}

//显示测试
void VFD_Display_Test(void)
{
    int i;
    send_cs();
    Write_PT6312(0xc0);  // Set Address
    for (i=0;i<9;i++)
    {
        Write_PT6312(vfdmap[i]); // Transfer display data low byte 
        Write_PT6312(0x00);        // High byte data transmission display
    }
    send_cs(); 
    Write_PT6312(0x80|ON|LIGHT); // Display Control
    VFD_STB_H();
    
}
// 打开LED显示

void VFD_LED_ON()
{
    send_cs();
    Write_PT6312(0x41); // 设定LED
    Write_PT6312(0x0C); // led1&LED2
    VFD_STB_H();    
}

// 关闭LED显示
void VFD_LED_OFF()
{
    send_cs();
    Write_PT6312(0x41); // 设定LED
    Write_PT6312(0x0F); // led1&LED2
    VFD_STB_H();    
}
void VFD_Display()
{
    int i;
    u8* p = DisplayCache;
    send_cs();
    Write_PT6312(0xc0);
    for (i=0;i<9;i++)
    {
        Write_PT6312(*p++);
        Write_PT6312(0x00);
    }
    send_cs(); 
    Write_PT6312(0x80|ON|LIGHT);
    VFD_STB_H();
}



/**********************函数定义结束*************************/

/******************* (C) COPYRIGHT 2019 TerayTechEnterprise *****END OF FILE****/
