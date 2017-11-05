#include  <msp430x14x.h>

#define uint unsigned int
#define uchar unsigned char

#define NCS_ON P5OUT &= ~BIT0  //P5.0==NCS
#define NCS_OFF P5OUT |= BIT0
#define SCLK_UP P5OUT |= BIT3
#define SCLK_DOWN P5OUT &= ~BIT3

#define Motion              Motion_Burst_Read_Buf[0]
#define Observation         Motion_Burst_Read_Buf[1] 
#define Delta_X_L           Motion_Burst_Read_Buf[2]
#define Delta_X_H           Motion_Burst_Read_Buf[3]
#define Delta_Y_L           Motion_Burst_Read_Buf[4]
#define Delta_Y_H           Motion_Burst_Read_Buf[5]
#define SQUAL               Motion_Burst_Read_Buf[6]
#define Pixel_Sum           Motion_Burst_Read_Buf[7]
#define Maximum_Pixel       Motion_Burst_Read_Buf[8]
#define Minimum_Pixel       Motion_Burst_Read_Buf[9]
#define Shutter_Upper       Motion_Burst_Read_Buf[10]
#define Shutter_Lower       Motion_Burst_Read_Buf[11]
#define Frame_Period_Upper  Motion_Burst_Read_Buf[12]
#define Frame_Period_Lower  Motion_Burst_Read_Buf[13]

uchar Motion_Burst_Read_Buf[14] = {0x00};//restore Motion burst read 14 bytes
uchar Powerup_step5[5] = {0xff};//restore 0x02-0x06 5 bytes
uchar CRC_16[2] = {0x00};//for CRC 0: upper, 1: lower
uchar First_Pixel_Check;
uchar PID = 0xff,VID = 0xff;
uchar SROM_ID;
uchar OBS;
uchar LOP;
uchar Squal;
uchar Infor_Buf[5];

int X,Y;
uchar Flag_X,Flag_Y;

/*******************************************
函数名称：Delayus
功    能：延时N个1us的时间
参    数：n--延时长度
返回值  ：无
********************************************/
void Delayus(uint x)//accurate 1.25*x + 0.25 us
{ 
    uint i,j;
    for(j=x;j>0;j--)
       for(i=3;i>0;i--);
}

/*******************************************
函数名称：Delayms
功    能：延时N个1ms的时间
参    数：n--延时长度
返回值  ：无
********************************************/
void Delayms(uint x)
{
    uint i,j,k;
    for(k=x;k>0;k--)//1.25*x 
      for(j=640;j>0;j--)
        for(i=3;i>0;i--);
}

/*******************************************
函数名称：SPI_Port_Initial
功    能：SPI串口初始化
参    数：无
返回值  ：无
********************************************/
void SPI_Port_Initial()
{
    P5SEL = 0x00;   //I/O FUNCTION 
    P5DIR = 0x00;   //INPUT
    P5DIR |= BIT0;  //P5.0 == NCS OUTPUT
    P5DIR |= BIT1;  //P5.1==SIMO OUTPUT
    P5DIR &= ~BIT2; //P5.2==SOMI INPUT
    P5DIR |= BIT3;  //P5.3==SCLK OUTPUT 
}

/*******************************************
函数名称：SPI_Read_One_Data
功    能：读某个地址的一个字节
参    数：一个字节的地址，针对某一个寄存器的地址
返回值  ：一个字节的数据
********************************************/
uchar SPI_Read_One_Data(uchar addr)//SCLK should be UP before used ,after used the SCLK is UP
{
    uchar data = 0;
    uchar data_buf;
    int i;
    
    addr = (addr&0x7f);//msb is 0
    for(i=0;i<8;i++)
    {
        SCLK_DOWN;//
        if(addr&BIT7)
            P5OUT |= BIT1;//msb = 1
        else
            P5OUT &= ~BIT1;//MSB =0
        Delayus(1);//t-setup
        SCLK_UP;//read at rising edge
        Delayus(1);//t-hold
        addr <<= 1;
    }//end of send addr
    
    Delayus(100);//t-SRAD
    
    for(i=0;i<8;i++)
    {
        SCLK_DOWN;//ANDS9500 send    
        if(P5IN&BIT2)//p3.2
          data_buf = 0x01;
        else
          data_buf = 0x00;
        SCLK_UP;
        Delayus(1);//t-hold
        data <<= 1;
        data = (data|data_buf);
    } //end of receive data
    return data;
}

/*******************************************
函数名称：SPI_Write_Addr_Data
功    能：向一个地址，写一个字节
参    数：地址，待发送的数据
返回值  ：无
********************************************/
void SPI_Write_Addr_Data(uchar addr, uchar data)
{
    uint i;
    
    addr = (addr|0x80);//for data write, msb is 1 
    for(i=0;i<8;i++)
    {
        SCLK_DOWN;
        if(addr&BIT7)
            P5OUT |= BIT1;//msb = 1
        else
            P5OUT &= ~BIT1;//MSB =0
        Delayus(1);//t-setup
        SCLK_UP;//read at rising edge
        Delayus(1);//t-hold
        addr <<= 1;
    }//end of send addr  
    
    for(i=0;i<8;i++)
    {
        SCLK_DOWN;
        if(data&BIT7)
            P5OUT |= BIT1;//msb = 1
        else
            P5OUT &= ~BIT1;//MSB =0
        Delayus(1);//t-setup
        SCLK_UP;//read at rising edge
        Delayus(1);//t-hold
        data <<= 1;
    }//end of send data
}

/*******************************************
函数名称：Burst_Mode_2_SROM_Download
功    能：采用Burst方式连续写，进行SROM下载
          将3070个字节的固件数据写入芯片
参    数：无
返回值  ：无
********************************************/
void Burst_Mode_2_SROM_Download()
{
    uint i,j;
    uchar addr,data;
    
    addr = (0x62|0x80);//for data write, msb is 1 
    for(i=0;i<8;i++)
    {
        SCLK_DOWN;
        if(addr&BIT7)
            P5OUT |= BIT1;//msb = 1
        else
            P5OUT &= ~BIT1;//MSB =0
        Delayus(1);//t-setup
        SCLK_UP;//read at rising edge
        Delayus(1);//t-hold
        addr <<= 1;
    }//end of send addr 
    Delayus(15);
    //send 3070 bytes
    for(j=0;j<3070;j++)
    {
        data = adns9500_srom_text_file[j];
        for(i=0;i<8;i++)
        {
            SCLK_DOWN;
            if(data&BIT7)
                P5OUT |= BIT1;//msb = 1
            else
                P5OUT &= ~BIT1;//MSB =0
            Delayus(1);//t-setup
            SCLK_UP;//read at rising edge
            Delayus(1);//t-hold
            data <<= 1;
        } 
        Delayus(15);  
    }
    NCS_OFF;//exit burst mode
    Delayus(4);//tBEXIT 500ns
}

/*******************************************
函数名称：Test_SROM_CRC
功    能：SROM下载完成后进行校验，是否成功
参    数：无
返回值  ：无
********************************************/
void Test_SROM_CRC()//restored in CRC_16[]
{
    NCS_ON;
    Delayus(20);
    SCLK_UP;

    SROM_ID = SPI_Read_One_Data(0x2a);//SROM ID read;        
    Delayus(20);
    
    //step1 == Write 0x15 to SROM_Enable reg to start SROM CRC test
    SPI_Write_Addr_Data(0x13,0x15);
    //step2 == wait at least 10ms
    Delayms(10);
    //step3 == Read CRC value from Data_Out_Lower and Data_Out_Upper regs
    CRC_16[0] = SPI_Read_One_Data(0x26);//upper 
    Delayus(20);//t-SRR
    CRC_16[1] = SPI_Read_One_Data(0x25);//lower  
}

/*******************************************
函数名称：SROM_Download
功    能：进行SROM下载
参    数：无
返回值  ：无
********************************************/
void SROM_Download()
{
    NCS_ON;
    Delayus(20);
    SCLK_UP;
    
    //step1 == select 3K bytes SROM size at Configuration_IV reg
    SPI_Write_Addr_Data(0x39,0x02);
    Delayus(120);//t-SWW
    //step2 == Write 0x1d to SROM_Enable reg for initializng 0x13
    SPI_Write_Addr_Data(0x13,0x1d);
    //step3 == wait for one frame
    Delayus(1000);
    //step4 == write 0x18 SROM_Enable reg again to start SROM downloading
    SPI_Write_Addr_Data(0x13,0x18);
    Delayus(120);
    //step5 == Star Downloading SROM, and exit Burst mode, off NCS
    Burst_Mode_2_SROM_Download();
  
    Delayus(10);
    NCS_OFF;
    Delayus(1);//t-BEXIT
}

/*******************************************
函数名称：LASER_Output_Power
功    能：驱动激光器
参    数：无
返回值  ：无
********************************************/
void LASER_Output_Power()
{
    LOP = SPI_Read_One_Data(0x20);
    LOP &= 0xf4;//1111 0100
    Delayus(20);//t-SRW
    SPI_Write_Addr_Data(0x20,LOP);    
    Delayus(120);//t-SWW/SWR 
}

/*******************************************
函数名称：Power_Up
功    能：芯片上电序列
参    数：无
返回值  ：无
********************************************/
void Power_Up()
{
    //step1 == apply power
    //step2 == reset SPI
    NCS_OFF;
    Delayus(20);
    NCS_ON;
    Delayus(20);
    //step3 == write 0x5a to Power_Up_Reset(0x3a)
    SCLK_UP;
    SPI_Write_Addr_Data(0x3a,0x5a);
    //step4 == wait at least 50ms
    Delayms(50);
    
    //step5 == read 0x02 0x03 0x04 0x05 0x06
    Powerup_step5[0] = SPI_Read_One_Data(0x02);    
    Delayus(20);//SRR
    Powerup_step5[1] = SPI_Read_One_Data(0x03);    
    Delayus(20);    
    Powerup_step5[2] = SPI_Read_One_Data(0x04);    
    Delayus(20);    
    Powerup_step5[3] = SPI_Read_One_Data(0x05);    
    Delayus(20);  
    Powerup_step5[4] = SPI_Read_One_Data(0x06);    
    Delayus(20); 
    NCS_OFF;
    Delayus(20);
    NCS_ON;
    //step5.1 == PID read and VID read
//    PID = SPI_Read_One_Data(0x00);
//    Delayus(20);//SRR
//    VID = SPI_Read_One_Data(0x01);
    Delayus(20);
    //step6 == SROM download
    SROM_Download();
    //step6.1 == SROM CRC check
    Delayus(150);//soonest >=160
    Test_SROM_CRC();
    //step7 == Enable laser by setting Forced_Disable bit(bit-0)of LASER_CTRL0 register(address 0x20) to 0
    Delayus(20);//SRW
    LASER_Output_Power();//CW1 CW2 CW3, Forced_Disable bit
}

/*******************************************
函数名称：Burst_Mode_1_Motion_Read
功    能：采用Burst方式进行读数据
参    数：无
返回值  ：无
********************************************/
void Burst_Mode_1_Motion_Read()//data restored in Motion_Burst_Read_Buf[i]
{ 
    uint i,j;
    uint data;
    uint data_buf;
    for(j=0;j<14;j++)//clear Receive_Buf[]
    {       
        for(i=0;i<8;i++)
        {
            SCLK_DOWN;//ANDS9500 send    
            if(P5IN&BIT2)//p3.2
              data_buf = 0x01;
            else
              data_buf = 0x00;
            SCLK_UP;
            Delayus(1);//t-hold
            data <<= 1;
            data = (data|data_buf);
        } //end of receive data 
        Motion_Burst_Read_Buf[j] = data;
    }
}

/*******************************************
函数名称：Burst_Motion_Read
功    能：采用Burst方式进行读取Motion等14个字节的数据
参    数：无
返回值  ：无
********************************************/
void Burst_Motion_Read()
{
    //step1 == lower NCS
    NCS_ON;
    //step2 == send 0x50 to Motion_Burst reg 0x50
    SCLK_UP;
    SPI_Write_Addr_Data(0x50,0x50);
    //step3 == wait for one frame
    Delayus(1000);// 
    //step4 == read 14 bytes continusly, end with off NCS for tBEXIT
    Burst_Mode_1_Motion_Read();
    NCS_OFF;//pulling NCS high
    Delayus(1);//t-BEXIT
    
    //step5 == read new data from step1
    //step6 == write any value to Motion Reg to clear residual motion
    SPI_Write_Addr_Data(0x02,0xff); 
    Delayus(120);//t-SWW/SWR
}   

/*******************************************
函数名称：Infor_Read
功    能：采用单个字节读取方式进行读取Motion等14个字节的数据
参    数：无
返回值  ：无
********************************************/
void Infor_Read()
{
    int i;
  
    NCS_ON;
    SCLK_UP;        
    for(i=0;i<5;i++)
    {
        Infor_Buf[i] = SPI_Read_One_Data(0x02+i);
        Delayus(20);//t-SRR/SRW
    }
    Squal = SPI_Read_One_Data(0x07);
    Delayus(20);
    SPI_Write_Addr_Data(0x02,0x00);//write 0x00 to motion to clear
    Delayus(120);//t-SWW/SWR
    
    NCS_OFF;
}

/*******************************************
函数名称：Load_Configuration
功    能：在芯片正常工作前对一些必要的寄存器进行配置
参    数：无
返回值  ：无
********************************************/
void Load_Configuration()
{
    SPI_Write_Addr_Data(0x0f,0x12);//config 1 cpi = 1620
    Delayus(120);//t-SWW/SWR
    
    SPI_Write_Addr_Data(0x10,0x0c);//config 2 Rpt_Mode bit = 1 ,to set Y cpi, &Fixed_FR = 1
    //Fixed Frame rate 1958, frame rate 1958,frame period 0x5dc0,24000
    Delayus(120);//t-SWW/SWR   
    
    SPI_Write_Addr_Data(0x2f,0x12);//config 4 cpi = 1620
    Delayus(120);//t-SWW/SWR
}

/*******************************************
函数名称：Load_Configuration1
功    能：在芯片正常工作前对一些必要的寄存器进行配置
参    数：无
返回值  ：无
********************************************/
void Load_Configuration1()//cpi = 5040
{
    SPI_Write_Addr_Data(0x0f,0x38);//config 1 cpi = 5040
    Delayus(120);//t-SWW/SWR
    
    SPI_Write_Addr_Data(0x10,0x0c);//config 2 Rpt_Mode bit = 1 ,to set Y cpi, &Fixed_FR = 1
    //Fixed Frame rate 1958, frame rate 1958,frame period 0x5dc0,24000
    Delayus(120);//t-SWW/SWR   
    
    SPI_Write_Addr_Data(0x2f,0x38);//config 4 cpi = 5040
    Delayus(120);//t-SWW/SWR
}

/*******************************************
函数名称：Set_Observation
功    能：在芯片正常工作前对observation寄存器进行配置
参    数：无
返回值  ：无
********************************************/
void Set_Observation()
{
    // Clear observation register
    SPI_Write_Addr_Data(0x24,0x00);
    Delayus(20);//t-SRR/SRW
}


/*******************************************
函数名称：Motion_Deal
功    能：将读取的数据进行处理,并进行补码转换
参    数：无
返回值  ：无
********************************************/
void Motion_Deal()
{
    X = Infor_Buf[2] << 8 | Infor_Buf[1]; //Combine MSB and LSB of X Data output register
    Y = Infor_Buf[4] << 8 | Infor_Buf[3]; //Combine MSB and LSB of Z Data output register
}