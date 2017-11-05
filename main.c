/*
void Simu_Port_Initial()
{
    P1DIR &= ~BIT0;//P1.0 INPUT 
    P5SEL = 0X00;
    P5DIR = 0X00;//ALL FOR INPUT
    P5DIR |= BIT2;//P3.2 OUTPUT CONNECT TO miso
}

uchar Simu_SPI_Read()//read one byte
{
    uchar NCS_Judge;
    uchar SCLK_Judge;
    uchar MOSI;
    uchar data_buf;
    uchar data;
    uchar i;
    
    do
      NCS_Judge = P1IN & 0x01;//p1.0
    while(NCS_Judge);//wait for NCS_ON, wait for low
    for(i=0;i<8;i++)
    {
      do  
      {
        SCLK_Judge = P5IN & 0x80;//p5.3
        if(SCLK_Judge)
          Delayus(2);
        else
          ;
      }
      while(!SCLK_Judge);//wait for raising edge, wait for high
      
      MOSI = P5IN & 0x02;
      if(MOSI)
        data_buf = 0x01;
      else
        data_buf = 0x00;
      data <<= 1;
      data = (data|data_buf);
    }    
    return data;
}

void Simu_SPI_Send(uchar data)//one byte
{
    uchar NCS_Judge;
    uchar SCLK_Judge;
    uchar i;
    
    do
      NCS_Judge = P1IN & 0x01;//p1.0
    while(NCS_Judge);//wait for NCS_ON
    for(i=0;i<8;i++)
    {
       do
         SCLK_Judge = P5IN & 0X80;
       while(SCLK_Judge);//wait for falling edge
       
       P3OUT = data & 0x80;
       data <<= 1;       
    }      
}

void Port_Initial()
{
    NCS_OFF;
    P5DIR = 0x00;//INPUT
    P5SEL = 0x00;//I/O FUNCTION
    P5DIR |= BIT1;//P5.1==SIMO OUTPUT
    P5DIR &= ~BIT2;//P5.2==SOMI INPUT
    P5DIR |= BIT3;//P5.3==SCLK OUTPUT   
}


void Adns9500_SPI_Addr_Write(uchar addr)//for read
{
    uchar addr_deal;
    uint i;
    
    Delayus(100);
    
    NCS_OFF;
    Delayus(10);
    NCS_ON;
    addr = addr & Read;//MSB = 0
    for(i=0;i<8;i++)
    {
        SCLK_DOWN;
        addr_deal = addr & 0x80;//send from MSB
        if(addr_deal)// 1000 0000b
        {
           P5OUT |= BIT1;//P5.1 OUTPUT 1
        }
        else
        {
            P5OUT &= ~BIT1;//P5.1 OUTPUT 0
        }
        Delayus(10);
        SCLK_UP;
        addr <<= 1;       
    }
    SCLK_UP;
    Delayus(200);
}

uchar Adns9500_SPI_Data_Read()
{
    uchar data = 0;
    uchar data_temp;
    uchar data_buf;
    uint i;
    
    NCS_ON;
//    P3OUT |= BIT2;//P3.2, MISO SET HIGH    ***
    for(i=0;i<8;i++)
    {
        SCLK_UP;
        Delayus(5);
        SCLK_DOWN;
        Delayus(2);    
        data_temp = P5IN;//p3.2
        if(data_temp & 0x04)
          data_buf = 0x01;
        else
          data_buf = 0x00;
        data <<= 1;
        data = (data|data_buf);              
    } 
    SCLK_UP;
    return data;
}

void Adns9500_SPI_AddrWrite_DataRead(uchar addr, uchar data)
{
    uchar addr_deal;
    uchar data_deal;
    uint i;
  
    NCS_OFF;
    Delayus(10);
    NCS_ON;
    addr = addr | Write;//for data read
    for(i=0;i<8;i++)
    {
        SCLK_DOWN;
        addr_deal = addr & 0x80;//send from MSB
        if(addr_deal)// 1000 0000b
        {
           P5OUT |= BIT1;//P3.1 OUTPUT 1
        }
        else
        {
            P5OUT &= ~BIT1;//P3.1 OUTPUT 0
        }
        Delayus(5);
        SCLK_UP;
        addr <<= 1;    
    }
    
    Delayus(10);//should be deleted while test end
    
    for(i=0;i<8;i++)
    {
        SCLK_DOWN;
        data_deal = data & 0x80;
        if(data_deal)// 1000 0000b
        {
           P5OUT |= BIT1;//P3.1 OUTPUT 1
        }
        else
        {
            P5OUT &= ~BIT1;//P3.1 OUTPUT 0
        }
        Delayus(5);
        SCLK_UP;
        data <<= 1;  
    }
    SCLK_UP;
    NCS_OFF;    
}
*/

/*
#pragma vector=USART1RX_VECTOR
__interrupt void SPI1_Rx (void)
{
  if(y < ReceiNum)
  {
    Receive_Buf[y] = RXBUF1;
    y++;
  }
  else
    IE2 &= ~URXIE1;
}


#pragma vector=USART1TX_VECTOR
__interrupt void SPI1_tx (void)//IFG clear auto
{
  if(x < TransNum) 
  {
    TXBUF1 = Send_Buf[x];
    Send_Buf[x] = 0x00;
    x++;
  }
  else
    IE2 &= ~UTXIE1;
}
*/


/*
void Adns9500_SPI_Initial()
{
    P5SEL |= 0X0E;
    P5DIR &= ~0X01;
//    NCS_OFF;//reset slave
//    NCS_ON;
    U1CTL = CHAR + SYNC + MM + SWRST;
    U1TCTL = CKPL + SSEL1 + STC;
    U1BR0 = 0X08;//SMCLK/8
    U1BR1 = 0X00;
    U1MCTL = 0x00;
    ME2 |= USPIE1;
    U1CTL &= ~SWRST;
    IE2 |= UTXIE1 + URXIE1;//TX and RX
}

void SPI_Write_Addr_Data(uchar addr,uchar data)//Adns9500_SPI_Initial should be used before
{
    addr |= 0x80;//MSB is 1 
    Send_Buf[0] = addr;
    Send_Buf[1] = data;
    TransNum = 2;
    IE2 |= UTXIE1;
    _EINT();
    while(x<TransNum);
    IE2 &= ~UTXIE1;
    _DINT();    
    x = 0;//recover j
}

uchar SPI_Read_One_Data(uchar addr)
{
    uchar data;
    addr &= 0x7f;//MSB is 0
    Send_Buf[0] = addr;
    TransNum = 1;
    ReceiNum = 1;
    IE2 |=  UTXIE1 + URXIE1;//open RX as well
    _EINT();
    while((x<TransNum)||(y<ReceiNum));//Send and Receive are both over
    IE2 &= ~UTXIE1;
    IE2 &= ~URXIE1;//close
    _DINT();
    x = 0;
    y = 0;
    data = Receive_Buf[0];
    Receive_Buf[0] = 0;
    return data;    
}
*/

/*
void LASER_Output_Power()
{
    //step1 == power up reset the mouse system
  
    //step2 == Enable the laser by setting the Forced_Disabled bit of LASER_CTRL0 register(0x20) to 0
    //step3 == Enable the Calibration mode by writing 010b to bits [3,2,1] of LASER_CTRL0 register(0x20)
    SPI_Write_Addr_Data(0x20,0x04);//CW1 CW2 CW3, Forced_Disable bit=0
    //step4 == measure at the LOP navigation surface plane
    
}



////////////////////////////////////////////////////////////////////
void Frame_Capture()
{
    NCS_ON;
    //step1 == reset chip by writing 0x5a to power_Up_Reset reg 0x3a
    SPI_Write_Addr_Data(0x3a,0x5a);
    //step2 == Enable laser by setting Forced_Disable bit(bit-0) 0f LASER_CTRL0 reg to 0
    SPI_Write_Addr_Data(0x20,0x04);//CW1 CW2 CW3, Forced_Disable bit=0
    //step3 == Write 0x93 to Frame_Capture reg
    SPI_Write_Addr_Data(0x12,0x93);
    //step4 == write 0xc5 to Frame_Capture reg
    SPI_Write_Addr_Data(0x12,0xc5);
    //step5 == wait for two frames
    Delayus(2000);
    //step6 == Check first pixel by reading bit-0 of Motion reg. if=1, first pixel is available
    First_Pixel_Check = SPI_Read_One_Data(0x02);//if bit-0 = 1 right
    //step7 == continue read from Pixel_Burst reg until 900 pixel are transferred
    
    //step8 == Exit
    NCS_OFF;
    Delayus(4);
}
////////////////////////////////////////////////////////////////////
*/

/***************************************************
程序功能：静态显示各种字符
----------------------------------------------------
测试说明：观察液晶显示
****************************************************/

#include  <msp430x14x.h>
#include  <math.h>
#include  <stdio.h>
#include  <intrinsics.h>

#include  "cry1602.h"
#include  "Cry1602.c"
#include  "SROM.c"
#include  "HMC5983.c"
#include  "ADNS9500.c"
#include  "12864.c"

#define uint unsigned int
#define uchar unsigned char


uchar shuzi[] = {"0123456789"};
uchar zimu[] = {"ABCDEFGHIGKLMNOPQRSTUVWXYZ"};
uchar display[2] = {"aa"}; 
uchar test[3] = {0x4f,0x80,0xf9};
uchar receive[3];

double HMC_Value_Standard;//
double HMC_Value;
double Delta_Angle;
double Motion_X;
double Motion_Y;

/*******************************************
函数名称：DisplayDeal
功    能：将要显示的一个字节的数据转换为可让液晶显示的16进制的数据字符
参    数：无
返回值  ：无
********************************************/
void DisplayDeal(uchar num)
{
    uchar temp;
    temp = num / 16;
    if(temp < 10)
      display[0] = shuzi[temp];
    else
      display[0] = zimu[temp-10];
    num = num % 16;
    if(num < 10)
      display[1] = shuzi[num];
    else
      display[1] = zimu[num-10];
}

/*******************************************
函数名称：HMC_ADNS_Port_Initial
功    能：对电子罗盘和ADNS9500进行端口初始化
参    数：无
返回值  ：无
********************************************/
void HMC_ADNS_Port_Initial()
{
    P5SEL = 0x00;//I/O FUNCTION 
    P5DIR = 0x00;//INPUT
    //HMC_5983
    P5DIR |= BIT6;//P5.6 == SCL
    P5DIR |= BIT7;//P5.7== SDA 
    //ADNS_9500 
    P5DIR |= BIT0;//P5.0 == NCS OUTPUT
    P5DIR |= BIT1;//P5.1==SIMO OUTPUT
    P5DIR &= ~BIT2;//P5.2==SOMI INPUT
    P5DIR |= BIT3;//P5.3==SCLK OUTPUT 
    
    Single_Write_HMC5883(0x02,0x00);  //HMC
    
    Power_Up();//ADNS_9500
    Load_Configuration1();//set cpi
    Set_Observation();//clear
}

/*******************************************
函数名称：main
功    能：主函数
参    数：无
返回值  ：无
********************************************/
void main( void )
{
    uint i;
    int Start_X = 64;
    int Start_Y = 32;
    int End_X;
    int End_Y;
//    double Remain_X = 0;
//    double Remain_Y = 0;
//    int remain_x;
//    int remain_y;

    //关闭看门狗
    WDTCTL = WDTPW + WDTHOLD;   
    //将上电默认的800KHz的MCLK时钟设置为外部高频晶振8M
    BCSCTL1 &= ~XT2OFF;
    do
    {
        IFG1 &= ~OFIFG;
        for(i=0xff;i>0;i--);
    }
    while((IFG1 & OFIFG));
    BCSCTL2 |= SELM_2;
  
    //下面六行程序关闭所有的IO口
    P1DIR = 0XFF;P1OUT = 0XFF;//output high
    P2DIR = 0XFF;P2OUT = 0XFF;
    P3DIR = 0XFF;P3OUT = 0XFF;
    P4DIR = 0XFF;P4OUT = 0XFF;
    P5DIR = 0XFF;P5OUT = 0XFF;
    P6DIR = 0XFF;P6OUT = 0XFF;

    HMC_ADNS_Port_Initial();  
//////////////////////////LCD12864/////////////////////////

    Ini_Lcd(); 
    Write_Cmd(0x01);//clear    
    Delay_Nms(10);
    GUI_Fill_GDRAM(0x00);//clear screen
    Delay_Nms(5);
    GUI_Point(64,32,1);
    
    //////////////////////data deal///////////////////////
    Multiple_Read_HMC5883();      //连续读出数据，存储在BUF中    
    HMC_Value_Standard = Angle_Deal();//确定角度的标准
    
    while(1)
    {
      
        Infor_Read();//ADNS-9500 
        Motion_Deal();
        
        Multiple_Read_HMC5883();      //连续读出数据，存储在BUF中
        HMC_Value = Angle_Deal();
        
        Delta_Angle = ((HMC_Value - HMC_Value_Standard)/360)*3.14159;//delta angle
        
        HMC_Value_Standard = HMC_Value;//for next
        
        Motion_X = (X * cos(Delta_Angle) - Y * sin(Delta_Angle)) / 198.43;//uint is cm
        Motion_Y = (X * sin(Delta_Angle) + Y * cos(Delta_Angle)) / 198.43;
/*        
        remain_x = Motion_X;//取整
        remain_y = Motion_Y;
        
        if(Remain_X>1||Remain_X<-1)
        {
            if(Remain_X>1)
                Remain_X = Remain_X-1;
            else
                Remain_X = Remain_X+1;
        }
        else
            Remain_X = Remain_X + Motion_X - remain_x;
            
        if(Remain_Y>1||Remain_Y<-1)
        {
            if(Remain_Y>1)
                Remain_Y = Remain_Y - 1;
            else
                Remain_Y = Remain_Y + 1;
        }
        else
            Remain_Y = Remain_Y + Motion_Y - remain_y;
        */
        
        End_X = Start_X + Motion_X + 0.5;//斯四舍五入 
        End_Y = Start_Y + Motion_Y + 0.5;//斯四舍五入
        
        ////////////display deal////////////
        Draw_Any_Line(Start_X,Start_Y,End_X,End_Y,1);//start x,y,end x,y,color
        Start_X = End_X;
        Start_Y = End_Y;
        Delayms(200);
       
        
    }
}

