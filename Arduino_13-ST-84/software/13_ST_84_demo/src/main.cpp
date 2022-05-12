#include <Arduino.h>

int VFD_RST = 3; //VFD reset
int VFD_CS = 4;  //VFD chip select
int VFD_SCK = 5; //VFD SPI SCLK
int VFD_DAT = 6; //VFD SPI MOSI
int VFD_PWR = 7; //VFD lamp and HV power control

#define AD_TIME 0x01
#define AD_SHIFT 0x02
#define AD_CLOCK 0x03
#define AD_HD 0x04
#define AD_USB 0x05
#define AD_LOCK 0x06
#define AD_DOLBY 0x07
#define AD_MUTE 0x08
#define AD_TU1 0x09
#define AD_TU2 0x0a
#define AD_MP3 0x0b
#define AD_LOOP 0x0c
#define AD_COLON1 0x10
#define AD_COLON2 0x11
#define AD_COLON3 0x12

//G1,G2,G3,G4,G5,G6,G7,G8,G9,G10,G11,G12,G13
unsigned char VFD_DCRAM[13] = {0x07, '1', '3', '-', 'S', 'T', '-', '8', '4', 'G', 'I', 'N', 'K'}; //DCRAM
unsigned char VFD_CGRAM[8][5] = {                                                                 //CGRAM
    0x08, 0x1c, 0x3e, 0x00, 0x3e,                                                                 //left arrow
    0x3e, 0x00, 0x3e, 0x1c, 0x08,                                                                 //right arrow
    0x28, 0x2c, 0x2e, 0x2c, 0x28,                                                                 //up arrow
    0x0a, 0x1a, 0x2a, 0x1a, 0x0a,                                                                 //down arrow
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x08, 0x00, 0x00};
//NC,TIME_D1,SHIFT_D1,CLOCK_D1,HD_D1,USB_D1,LOCK_D1,DOLBY_D1,MUTE_D1,TU1_D1,TU2_D1,MP3_D1,LOOP_D1,NC,NC,NC,:1_D0,:2_D0,:3_d0
unsigned char VFD_ADRAM[19] = {0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //ADRAM

unsigned int VFD_BRT_DATA; // display duty

void GPIO_Init()
{
  pinMode(VFD_RST, OUTPUT);
  pinMode(VFD_CS, OUTPUT);
  pinMode(VFD_SCK, OUTPUT);
  pinMode(VFD_DAT, OUTPUT);
  pinMode(VFD_PWR, OUTPUT);
}

//Wtite data to VFD's CIG
void VFD_Write_Data(unsigned char DAT)
{
  unsigned char i;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(VFD_SCK, LOW);
    digitalWrite(VFD_DAT, DAT & 0x01);
    //VFD_DAT = DAT & 0x01;
    delayMicroseconds(10);
    digitalWrite(VFD_SCK, HIGH);
    delayMicroseconds(10);
    DAT >>= 1;
  }
}

//Initialize the CIG
void VFD_Init(void)
{
  unsigned char i, j;

  digitalWrite(VFD_RST, LOW); //reset the CIG
  delay(5);
  digitalWrite(VFD_RST, HIGH);
  delay(5);

  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0xe0); //Set the display timing
  VFD_Write_Data(0xac); //
  digitalWrite(VFD_CS, HIGH);

  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0x80); //Write URAM
  VFD_Write_Data(0xc0); //T17 G7 G8
  VFD_Write_Data(0x00);
  VFD_Write_Data(0x00); //T18 G9 G10
  VFD_Write_Data(0x03);
  VFD_Write_Data(0x00); //T19 G11 G12
  VFD_Write_Data(0xc0);
  digitalWrite(VFD_CS, HIGH);

  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0xe4); //Set the dimming data
  VFD_Write_Data(200);
  digitalWrite(VFD_CS, HIGH);

  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0x40); //write CGRAM
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 5; j++)
    {
      VFD_Write_Data(VFD_CGRAM[i][j]); //CGRAM
    }
  }
  digitalWrite(VFD_CS, HIGH);

  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0x20); //write DCRAM
  for (i = 0; i < 13; i++)
  {
    VFD_Write_Data(VFD_DCRAM[i]); //DCRAM
  }
  digitalWrite(VFD_CS, HIGH);

  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0X60); //write ADRAM
  for (i = 0; i < 19; i++)
  {
    VFD_Write_Data(VFD_ADRAM[i]); //ADRAM
  }
  digitalWrite(VFD_CS, HIGH);

  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0xe8); //all display on
  digitalWrite(VFD_CS, HIGH);
}

//set VFD display on or off , 0-off , 1-on
void VFD_Disp_On_Off(bool on_off_flag)
{
  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0xe8 | (((unsigned char)(!on_off_flag)) << 1));
  digitalWrite(VFD_CS, HIGH);
}

//Set the display dimming data
void VFD_Brt_Set(unsigned char BRT_DAT)
{
  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0xe4); //display dimming set command
  VFD_Write_Data(BRT_DAT);
  digitalWrite(VFD_CS, HIGH);
}

//Load DCRAM buffer to VFD's DCRAM
void VFD_DCRAM_Load(void)
{
  unsigned char i;
  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0x20); //write DCRAM
  for (i = 0; i < 13; i++)
  {
    VFD_Write_Data(VFD_DCRAM[i]); //DCRAM
  }
  digitalWrite(VFD_CS, HIGH);
}

//Write data to fixed DCRAM address
void VFD_Write_DCRAM(unsigned char addr, unsigned char dat)
{
  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0x20 | addr); //write DCRAM
  VFD_Write_Data(dat);         //DCRAM
  digitalWrite(VFD_CS, HIGH);
}

//Load CGRAM buffer to VFD's CGRAM
void VFD_CGRAM_Load(void)
{
  unsigned char i, j;
  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0x40); //write CGRAM
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 5; j++)
    {
      VFD_Write_Data(VFD_CGRAM[i][j]); //CGRAM
    }
  }
  digitalWrite(VFD_CS, HIGH);
}

//Write data to fixed CGRAM address
void VFD_Write_CGRAM(unsigned char addr, unsigned char *dat)
{
  unsigned char i;
  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0x40 | addr); //write CGRAM
  for (i = 0; i < 5; i++)
  {
    VFD_Write_Data(*dat++); //CGRAM
  }
  digitalWrite(VFD_CS, HIGH);
}

//Load ADRAM buffer to VFD's ADRAM
void VFD_ADRAM_Load(void)
{
  unsigned char i;
  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0x60); //ADRAM
  for (i = 0; i < 19; i++)
  {
    VFD_Write_Data(VFD_ADRAM[i]); //ADRAM
  }
  digitalWrite(VFD_CS, HIGH);
}

//Write data to fixed ADRAM address
void VFD_Write_ADRAM(unsigned char ad_dat, bool on_off_flag)
{
  unsigned char ad_dat_temp;
  digitalWrite(VFD_CS, LOW);
  VFD_Write_Data(0x60 | ad_dat); //ADRAM
  if ((ad_dat == AD_COLON1) || (ad_dat == AD_COLON2) || (ad_dat == AD_COLON3))
    ad_dat_temp = 0x01;
  else
    ad_dat_temp = 0x02;
  on_off_flag ? VFD_Write_Data(ad_dat_temp) : VFD_Write_Data(0x00); //ADRAM
  digitalWrite(VFD_CS, HIGH);
}

void setup()
{
  // put your setup code here, to run once:
  //unsigned char i;
  Serial.println("GPIO init");
  GPIO_Init();
  VFD_Init();
  Serial.begin(9600);

  Serial.println("VFD Init done");

  VFD_PWR = 0;

  delay(1000);

  VFD_Write_CGRAM(0x00, VFD_CGRAM[2]);

  VFD_Write_DCRAM(0x01, 0x00);

  VFD_Write_ADRAM(AD_TIME, 0);

  delay(100);

  VFD_Write_ADRAM(AD_TIME, 1);

  delay(100);

  VFD_Write_DCRAM(0x01, '1');

  VFD_Brt_Set(240);
  delay(1000);
  VFD_Brt_Set(255);

  for (int i = 0; i < 13; i++)
  {
    VFD_Write_DCRAM(i, 'A');
    delay(100);
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  ;
}