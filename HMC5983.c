#include  <msp430x14x.h>

#define SCL_H P5OUT |= BIT6
#define SCL_L P5OUT &= ~BIT6
#define SDA_H P5OUT |= BIT7
#define SDA_L P5OUT &= ~BIT7

#define SDA_in  P5DIR &= ~BIT7   //SDA�ĳ�����ģʽ
#define SDA_out P5DIR |= BIT7    //SDA������ģʽ
#define SDA_val P5IN&BIT7        //SDA��λֵ
 
#define TRUE    1
#define FALSE   0

#define	SlaveAddress   0x3C	  //����������IIC�����еĴӵ�ַ

uchar NUM[10]={"0123456789"};
uchar ge,shi,bai,qian,wan;           //��ʾ����
uchar BUF[8];

//double HMC_Value;
/***********************************************
�������ƣ�Delay1us
��    �ܣ���ʱ1us
��    ����x��ֵ������ʱNus
����ֵ  ��
***********************************************/
void Delay1us(uint x)//accurate 1.25*x + 0.25 us
{ 
    uint i,j;
    for(j=x;j>0;j--)
       for(i=3;i>0;i--);
}

/***********************************************
�������ƣ�conversion
��    �ܣ�������ת��Ϊ����Һ������ʾ�ĵ����ַ�
��    ����Ҫת��������
����ֵ  ��
***********************************************/
void conversion(uint temp_data)  
{  
    wan=temp_data/10000 + 0x30;
    
    temp_data=temp_data%10000;   //ȡ������
    qian=temp_data/1000 + 0x30;
    
    temp_data=temp_data%1000;    //ȡ������
    bai=temp_data/100 + 0x30;
    
    temp_data=temp_data%100;     //ȡ������
    shi=temp_data/10 + 0x30;
    
    temp_data=temp_data%10;      //ȡ������
    ge=temp_data + 0x30; 	
}


/***********************************************
�������ƣ�HMC5883_Start
��    �ܣ���ʼ�ź�
��    ������
����ֵ  ��
***********************************************/
void HMC5883_Start()
{
    SDA_H;                    //����������
    SCL_H;                    //����ʱ����
    Delay1us(5);              //��ʱ
    SDA_L;                    //�����½���
    Delay1us(5);              //��ʱ
    SCL_L;                    //����ʱ����
}

/***********************************************
�������ƣ�HMC5883_Stop
��    �ܣ�ֹͣ�ź�
��    ������
����ֵ  ��
***********************************************/
void HMC5883_Stop()
{
    SDA_L;                    //����������
    SCL_H;                    //����ʱ����
    Delay1us(5);                 //��ʱ
    SDA_H;                    //����������
    Delay1us(5);                 //��ʱ
}

/***********************************************
�������ƣ�HMC5883_SendACK1
��    �ܣ�����Ӧ���ź�
��    ����ack (0:ACK 1:NAK)
����ֵ  ��
***********************************************/
void HMC5883_SendACK1()
{
    SDA_H;                  //дӦ���ź�
    SCL_H;                    //����ʱ����
    Delay1us(5);                 //��ʱ
    SCL_L;                    //����ʱ����
    Delay1us(5);                 //��ʱ
}

/***********************************************
�������ƣ�HMC5883_SendACK0
��    �ܣ�����Ӧ���ź�
��    ����ack (0:ACK 1:NAK)
����ֵ  ��
***********************************************/
void HMC5883_SendACK0()
{
    SDA_L;                  //дӦ���ź�
    SCL_H;                    //����ʱ����
    Delay1us(5);                 //��ʱ
    SCL_L;                    //����ʱ����
    Delay1us(5);                 //��ʱ
}

/***********************************************
�������ƣ�HMC5883_RecvACK()
��    �ܣ�����Ӧ���ź�
��    ������
����ֵ  ��
***********************************************/
uchar HMC5883_RecvACK()
{
    uchar a;   
    SDA_in;
    
    SCL_H;                    //����ʱ����
    Delay1us(5);                 //��ʱ
    
    a = SDA_val;                 //��Ӧ���ź�
    SCL_L;                    //����ʱ����
    Delay1us(5);                 //��ʱ
    SDA_out;

    return a;
}

/***********************************************
�������ƣ�HMC5883_SendByte
��    �ܣ���IIC���߷���һ���ֽ�����
��    ���������͵����ݣ�һ���ֽ�
����ֵ  ��
***********************************************/
void HMC5883_SendByte(uchar dat)
{
    uchar i;

    for(i = 0;i < 8;i++)//8λ������
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
�������ƣ�HMC5883_RecvByt
��    �ܣ���IIC���߽���һ���ֽ�����
��    ������
����ֵ  ���յ���һ���ֽڵ�����
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
�������ƣ�Single_Write_HMC5883
��    �ܣ���ĳһ���Ĵ�������һ���ֽڵ�����
��    ����uchar REG_Address���Ĵ�����ַ
          uchar REG_data�������͵�����
����ֵ  ��
***********************************************/
void Single_Write_HMC5883(uchar REG_Address,uchar REG_data)
{
    HMC5883_Start();                  //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    HMC5883_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf 
    HMC5883_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf
    HMC5883_Stop();                   //����ֹͣ�ź�
}

/***********************************************
�������ƣ�Single_Read_HMC5883
��    �ܣ����ֽڶ�ȡ�ڲ��Ĵ���
��    �����Ĵ�����ַ
����ֵ  ��һ���ֽڵ�����
***********************************************/
uchar Single_Read_HMC5883(uchar REG_Address)
{  
    uchar REG_data;
    HMC5883_Start();                  //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    HMC5883_SendByte(REG_Address);    //���ʹ洢��Ԫ��ַ����0��ʼ	
    HMC5883_Start();                  //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress+1); //�����豸��ַ+���ź�
    REG_data = HMC5883_RecvByte();    //�����Ĵ�������
    HMC5883_SendACK1();   
    HMC5883_Stop();                   //ֹͣ�ź�
    return REG_data; 
}

/***********************************************
�������ƣ�Multiple_Read_HMC5883
��    �ܣ���������HMC5883�ڲ��Ƕ����ݣ���ַ��Χ0x3~0x5
��    ������
����ֵ  ����
***********************************************/
void Multiple_Read_HMC5883(void)
{   uchar i;
    HMC5883_Start();                  //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    HMC5883_SendByte(0x03);           //���ʹ洢��Ԫ��ַ����0x3��ʼ	
    HMC5883_Start();                  //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress+1); //�����豸��ַ+���ź�
    for (i=0; i<6; i++)               //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF[i] = HMC5883_RecvByte();  //BUF[0]�洢����
        if (i == 5)
        {
           HMC5883_SendACK1();        //���һ��������Ҫ��NOACK
        }
        else
        {
          HMC5883_SendACK0();         //��ӦACK
       }
   }
    HMC5883_Stop();                   //ֹͣ�ź�
    Delay1us(5000);
}

/***********************************************
�������ƣ�Init_HMC5883
��    �ܣ���ʼ��HMC5883
��    ������
����ֵ  ����
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
�������ƣ�Angle_Deal()
��    �ܣ��ֱ��ȡX�ᡢY�ᡢZ���ֵ���������ݽ��д�����������ʹ��
��    ������
����ֵ  ���ƾ����͵ĽǶ�ֵ
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
    conversion(angle);       //�������ݺ���ʾ
    return (angle/10);
}