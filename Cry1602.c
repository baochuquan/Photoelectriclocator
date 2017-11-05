#include  <msp430x14x.h>
#include "cry1602.h"

#define uint unsigned int
#define uchar unsigned char

/**************�궨��***************/
#define DataDir     P4DIR
#define DataPort    P4OUT				
#define Busy	    0x80
#define CtrlDir     P3DIR
#define CLR_RS P3OUT&=~BIT0;    //RS = P3.0	
#define SET_RS P3OUT|=BIT0;	
#define CLR_RW P3OUT&=~BIT1;	//RW = P3.1
#define SET_RW P3OUT|=BIT1;	
#define CLR_EN P3OUT&=~BIT2;	//EN = P3.2	
#define SET_EN P3OUT|=BIT2;	
/***********************************************
�������ƣ�DispStr
��    �ܣ���Һ����ĳ��λ����������ʾһ���ַ���
��    ����x--λ�õ�������
          y--λ�õ�������
          ptr--ָ���ַ������λ�õ�ָ��
����ֵ  ����
***********************************************/
void DispStr(uchar x,uchar y,uchar *ptr) 
{
    uchar *temp;
    uchar i,n = 0;
    
    temp = ptr;
    while(*ptr++ != '\0')   n++;    //�����ַ�����Ч�ַ��ĸ���
    
    for (i=0;i<n;i++)
    {
        Disp1Char(x++,y,temp[i]);
        if (x == 0x0f)
        {
           x  = 0; 
           y ^= 1;
        }
    }
}
/*******************************************
�������ƣ�DispNchar
��    �ܣ���Һ����ĳ��λ����������ʾN���ַ�
��    ����x--λ�õ�������
          y--λ�õ�������
          n--�ַ�����
          ptr--ָ���ַ����λ�õ�ָ��
����ֵ  ����
********************************************/
void DispNChar(uchar x,uchar y, uchar n,uchar *ptr) 
{
    uchar i;
    
    for (i=0;i<n;i++)
    {
        Disp1Char(x++,y,ptr[i]);
        if (x == 0x0f)
        {
           x = 0; 
        y ^= 1;
        }
    }
}
/*******************************************
�������ƣ�LocateXY
��    �ܣ���Һ��������ʾ�ַ�λ�õ�������Ϣ
��    ����x--λ�õ�������
          y--λ�õ�������
����ֵ  ����
********************************************/
void LocateXY(uchar x,uchar y) 
{
    uchar temp;

    temp = x&0x0f;
    y &= 0x01;
    if(y)   temp |= 0x40;  //����ڵ�2��
    temp |= 0x80;

    LcdWriteCommand(temp,1);
}
/*******************************************
�������ƣ�Disp1Char
��    �ܣ���ĳ��λ����ʾһ���ַ�
��    ����x--λ�õ�������
          y--λ�õ�������
          data--��ʾ���ַ�����
����ֵ  ����
********************************************/
void Disp1Char(uchar x,uchar y,uchar data) 
{
    LocateXY( x, y );			
    LcdWriteData( data );		
}
/*******************************************
�������ƣ�LcdReset
��    �ܣ���1602Һ��ģ����и�λ����
��    ������
����ֵ  ����
********************************************/
void LcdReset(void) 
{
    CtrlDir |= 0x07;                 //�����߶˿���Ϊ���״̬ 
    DataDir  = 0xFF;                 //���ݶ˿���Ϊ���״̬
  
    LcdWriteCommand(0x38, 0);	    //�涨�ĸ�λ����
    Delay5ms();
    LcdWriteCommand(0x38, 0);		
    Delay5ms();
    LcdWriteCommand(0x38, 0);
    Delay5ms();

    LcdWriteCommand(0x38, 1);		//��ʾģʽ����
    LcdWriteCommand(0x08, 1);		//��ʾ�ر�
    LcdWriteCommand(0x01, 1);	    //��ʾ����
    LcdWriteCommand(0x06, 1);		//д�ַ�ʱ���岻�ƶ�
    LcdWriteCommand(0x0c, 1);		//��ʾ���������α꣬����˸
}
/*******************************************
�������ƣ�LcdWriteCommand
��    �ܣ���Һ��ģ��д������
��    ����cmd--���
          chk--�Ƿ���æ�ı�־��1����æ��0������
����ֵ  ����
********************************************/
void LcdWriteCommand(uchar cmd,uchar chk) 
{

    if (chk) WaitForEnable();   // ���æ�ź�?
    
    CLR_RS;	
    CLR_RW; 
    _NOP();

    DataPort = cmd;             //��������д�����ݶ˿� 
    _NOP();					
    
    SET_EN;                     //����ʹ�������ź�
    _NOP();
    _NOP();
    CLR_EN;			
}
	
/*******************************************
�������ƣ�LcdWriteData
��    �ܣ���Һ����ʾ�ĵ�ǰ��ַд����ʾ����
��    ����data--��ʾ�ַ�����
����ֵ  ����
********************************************/
void LcdWriteData( uchar data ) 
{
    WaitForEnable();        //�ȴ�Һ����æ

    SET_RS;
    CLR_RW; 
    _NOP();

    DataPort = data;        //����ʾ����д�����ݶ˿�
    _NOP();

    SET_EN;                 //����ʹ�������ź�
    _NOP(); 
    _NOP(); 
    CLR_EN;		
}
/*******************************************
�������ƣ�WaitForEnable
��    �ܣ��ȴ�1602Һ������ڲ�����
��    ������
����ֵ  ����
********************************************/
void WaitForEnable(void) 
{
    P4DIR &= 0x00;  //��P4���л�Ϊ����״̬

    CLR_RS;
    SET_RW;
    _NOP();
    SET_EN; 
    _NOP();
    _NOP();
	
    while((P4IN & Busy)!=0);  //���æ��־

    CLR_EN;

    P4DIR |= 0xFF;  //��P4���л�Ϊ���״̬
}	
                       
/*******************************************
�������ƣ�Delay5ms
��    �ܣ���ʱԼ5ms
��    ������
����ֵ  ����
********************************************/
void Delay5ms(void)
{ 
    uint i=40000;
    while (i != 0)
    {
        i--;
    }
}
