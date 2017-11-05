#include  <msp430x14x.h>

#define SCL_H P5OUT |= BIT6
#define SCL_L P5OUT &= ~BIT6
#define SDA_H P5OUT |= BIT7
#define SDA_L P5OUT &= ~BIT7

#define SDA_in  P5DIR &= ~BIT7   //SDA改成输入模式
#define SDA_out P5DIR |= BIT7    //SDA变回输出模式
#define SDA_val P5IN&BIT7        //SDA的位值
 
#define TRUE    1
#define FALSE   0

#define	SlaveAddress   0x3C	  //定义器件在IIC总线中的从地址

uchar NUM[10]={"0123456789"};
uchar ge,shi,bai,qian,wan;           //显示变量
uchar BUF[8];

//double HMC_Value;
/***********************************************
函数名称：Delay1us
功    能：延时1us
参    数：x的值代表延时Nus
返回值  无
***********************************************/
void Delay1us(uint x)//accurate 1.25*x + 0.25 us
{ 
    uint i,j;
    for(j=x;j>0;j--)
       for(i=3;i>0;i--);
}

/***********************************************
函数名称：conversion
功    能：将数据转化为能让液晶屏显示的单个字符
参    数：要转换的数据
返回值  无
***********************************************/
void conversion(uint temp_data)  
{  
    wan=temp_data/10000 + 0x30;
    
    temp_data=temp_data%10000;   //取余运算
    qian=temp_data/1000 + 0x30;
    
    temp_data=temp_data%1000;    //取余运算
    bai=temp_data/100 + 0x30;
    
    temp_data=temp_data%100;     //取余运算
    shi=temp_data/10 + 0x30;
    
    temp_data=temp_data%10;      //取余运算
    ge=temp_data + 0x30; 	
}


/***********************************************
函数名称：HMC5883_Start
功    能：起始信号
参    数：无
返回值  无
***********************************************/
void HMC5883_Start()
{
    SDA_H;                    //拉高数据线
    SCL_H;                    //拉高时钟线
    Delay1us(5);              //延时
    SDA_L;                    //产生下降沿
    Delay1us(5);              //延时
    SCL_L;                    //拉低时钟线
}

/***********************************************
函数名称：HMC5883_Stop
功    能：停止信号
参    数：无
返回值  无
***********************************************/
void HMC5883_Stop()
{
    SDA_L;                    //拉低数据线
    SCL_H;                    //拉高时钟线
    Delay1us(5);                 //延时
    SDA_H;                    //产生上升沿
    Delay1us(5);                 //延时
}

/***********************************************
函数名称：HMC5883_SendACK1
功    能：发送应答信号
参    数：ack (0:ACK 1:NAK)
返回值  无
***********************************************/
void HMC5883_SendACK1()
{
    SDA_H;                  //写应答信号
    SCL_H;                    //拉高时钟线
    Delay1us(5);                 //延时
    SCL_L;                    //拉低时钟线
    Delay1us(5);                 //延时
}

/***********************************************
函数名称：HMC5883_SendACK0
功    能：发送应答信号
参    数：ack (0:ACK 1:NAK)
返回值  无
***********************************************/
void HMC5883_SendACK0()
{
    SDA_L;                  //写应答信号
    SCL_H;                    //拉高时钟线
    Delay1us(5);                 //延时
    SCL_L;                    //拉低时钟线
    Delay1us(5);                 //延时
}

/***********************************************
函数名称：HMC5883_RecvACK()
功    能：接收应答信号
参    数：无
返回值  无
***********************************************/
uchar HMC5883_RecvACK()
{
    uchar a;   
    SDA_in;
    
    SCL_H;                    //拉高时钟线
    Delay1us(5);                 //延时
    
    a = SDA_val;                 //读应答信号
    SCL_L;                    //拉低时钟线
    Delay1us(5);                 //延时
    SDA_out;

    return a;
}

/***********************************************
函数名称：HMC5883_SendByte
功    能：向IIC总线发送一个字节数据
参    数：待发送的数据，一个字节
返回值  无
***********************************************/
void HMC5883_SendByte(uchar dat)
{
    uchar i;

    for(i = 0;i < 8;i++)//8位计数器
    {
      
       if(dat & 0x80)//1
            SDA_H;            
        else 			    
            SDA_L;
        SCL_H;
        Delay1us(5);
        SCL_L;
        Delay1us(5);
        dat <<= 1;
    }    
    HMC5883_RecvACK();
}

/***********************************************
函数名称：HMC5883_RecvByt
功    能：从IIC总线接收一个字节数据
参    数：无
返回值  接收到的一个字节的数据
***********************************************/
uchar HMC5883_RecvByte()
{
    uchar  rdata = 0x00,i;
    uchar flag;
    
    SDA_H;
    for(i = 0;i < 8;i++)
    {
            SCL_H;
    SDA_in;
            Delay1us(5);
            flag = SDA_val;
            rdata <<= 1;
            if(flag)	
                rdata |= 0x01;
    SDA_out;
            SCL_L;
            Delay1us(5);
    }
    return rdata;
}


/***********************************************
函数名称：Single_Write_HMC5883
功    能：向某一个寄存器发送一个字节的数据
参    数：uchar REG_Address：寄存器地址
          uchar REG_data：待发送的数据
返回值  无
***********************************************/
void Single_Write_HMC5883(uchar REG_Address,uchar REG_data)
{
    HMC5883_Start();                  //起始信号
    HMC5883_SendByte(SlaveAddress);   //发送设备地址+写信号
    HMC5883_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf 
    HMC5883_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf
    HMC5883_Stop();                   //发送停止信号
}

/***********************************************
函数名称：Single_Read_HMC5883
功    能：单字节读取内部寄存器
参    数：寄存器地址
返回值  ：一个字节的数据
***********************************************/
uchar Single_Read_HMC5883(uchar REG_Address)
{  
    uchar REG_data;
    HMC5883_Start();                  //起始信号
    HMC5883_SendByte(SlaveAddress);   //发送设备地址+写信号
    HMC5883_SendByte(REG_Address);    //发送存储单元地址，从0开始	
    HMC5883_Start();                  //起始信号
    HMC5883_SendByte(SlaveAddress+1); //发送设备地址+读信号
    REG_data = HMC5883_RecvByte();    //读出寄存器数据
    HMC5883_SendACK1();   
    HMC5883_Stop();                   //停止信号
    return REG_data; 
}

/***********************************************
函数名称：Multiple_Read_HMC5883
功    能：连续读出HMC5883内部角度数据，地址范围0x3~0x5
参    数：无
返回值  ：无
***********************************************/
void Multiple_Read_HMC5883(void)
{   uchar i;
    HMC5883_Start();                  //起始信号
    HMC5883_SendByte(SlaveAddress);   //发送设备地址+写信号
    HMC5883_SendByte(0x03);           //发送存储单元地址，从0x3开始	
    HMC5883_Start();                  //起始信号
    HMC5883_SendByte(SlaveAddress+1); //发送设备地址+读信号
    for (i=0; i<6; i++)               //连续读取6个地址数据，存储中BUF
    {
        BUF[i] = HMC5883_RecvByte();  //BUF[0]存储数据
        if (i == 5)
        {
           HMC5883_SendACK1();        //最后一个数据需要回NOACK
        }
        else
        {
          HMC5883_SendACK0();         //回应ACK
       }
   }
    HMC5883_Stop();                   //停止信号
    Delay1us(5000);
}

/***********************************************
函数名称：Init_HMC5883
功    能：初始化HMC5883
参    数：无
返回值  ：无
***********************************************/
void Init_HMC5883()
{
      //HMC5983 Initial
     P5SEL = 0x00;//I/O FUNCTION 
     P5DIR = 0x00;//INPUT
     P5DIR |= BIT6;//P5.0 == NCS OUTPUT
     P5DIR |= BIT7;//P5.1==SIMO OUTPUT 
     
     Single_Write_HMC5883(0x02,0x00);  //
}


/***********************************************
函数名称：Angle_Deal()
功    能：分别读取X轴、Y轴、Z轴的值，并对数据进行处理让主函数使用
参    数：无
返回值  ：黄精度型的角度值
***********************************************/
double Angle_Deal()//return to Angle[]
{
    int x,y,z;
    double angle;  
    
    x = BUF[0] << 8 | BUF[1]; //Combine MSB and LSB of X Data output register
    z = BUF[2] << 8 | BUF[3]; //Combine MSB and LSB of Z Data output register
    y = BUF[4] << 8 | BUF[5]; //Combine MSB and LSB of Y Data output register
    angle = atan2((double)y,(double)x) * (180 / 3.14159265) + 180; // angle in degrees
    
//    HMC_Value = angle;//for main to deal
    
    angle *= 10;    
    conversion(angle);       //计算数据和显示
    return (angle/10);
}