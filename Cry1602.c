#include  <msp430x14x.h>
#include "cry1602.h"

#define uint unsigned int
#define uchar unsigned char

/**************宏定义***************/
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
函数名称：DispStr
功    能：让液晶从某个位置起连续显示一个字符串
参    数：x--位置的列坐标
          y--位置的行坐标
          ptr--指向字符串存放位置的指针
返回值  ：无
***********************************************/
void DispStr(uchar x,uchar y,uchar *ptr) 
{
    uchar *temp;
    uchar i,n = 0;
    
    temp = ptr;
    while(*ptr++ != '\0')   n++;    //计算字符串有效字符的个数
    
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
函数名称：DispNchar
功    能：让液晶从某个位置起连续显示N个字符
参    数：x--位置的列坐标
          y--位置的行坐标
          n--字符个数
          ptr--指向字符存放位置的指针
返回值  ：无
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
函数名称：LocateXY
功    能：向液晶输入显示字符位置的坐标信息
参    数：x--位置的列坐标
          y--位置的行坐标
返回值  ：无
********************************************/
void LocateXY(uchar x,uchar y) 
{
    uchar temp;

    temp = x&0x0f;
    y &= 0x01;
    if(y)   temp |= 0x40;  //如果在第2行
    temp |= 0x80;

    LcdWriteCommand(temp,1);
}
/*******************************************
函数名称：Disp1Char
功    能：在某个位置显示一个字符
参    数：x--位置的列坐标
          y--位置的行坐标
          data--显示的字符数据
返回值  ：无
********************************************/
void Disp1Char(uchar x,uchar y,uchar data) 
{
    LocateXY( x, y );			
    LcdWriteData( data );		
}
/*******************************************
函数名称：LcdReset
功    能：对1602液晶模块进行复位操作
参    数：无
返回值  ：无
********************************************/
void LcdReset(void) 
{
    CtrlDir |= 0x07;                 //控制线端口设为输出状态 
    DataDir  = 0xFF;                 //数据端口设为输出状态
  
    LcdWriteCommand(0x38, 0);	    //规定的复位操作
    Delay5ms();
    LcdWriteCommand(0x38, 0);		
    Delay5ms();
    LcdWriteCommand(0x38, 0);
    Delay5ms();

    LcdWriteCommand(0x38, 1);		//显示模式设置
    LcdWriteCommand(0x08, 1);		//显示关闭
    LcdWriteCommand(0x01, 1);	    //显示清屏
    LcdWriteCommand(0x06, 1);		//写字符时整体不移动
    LcdWriteCommand(0x0c, 1);		//显示开，不开游标，不闪烁
}
/*******************************************
函数名称：LcdWriteCommand
功    能：向液晶模块写入命令
参    数：cmd--命令，
          chk--是否判忙的标志，1：判忙，0：不判
返回值  ：无
********************************************/
void LcdWriteCommand(uchar cmd,uchar chk) 
{

    if (chk) WaitForEnable();   // 检测忙信号?
    
    CLR_RS;	
    CLR_RW; 
    _NOP();

    DataPort = cmd;             //将命令字写入数据端口 
    _NOP();					
    
    SET_EN;                     //产生使能脉冲信号
    _NOP();
    _NOP();
    CLR_EN;			
}
	
/*******************************************
函数名称：LcdWriteData
功    能：向液晶显示的当前地址写入显示数据
参    数：data--显示字符数据
返回值  ：无
********************************************/
void LcdWriteData( uchar data ) 
{
    WaitForEnable();        //等待液晶不忙

    SET_RS;
    CLR_RW; 
    _NOP();

    DataPort = data;        //将显示数据写入数据端口
    _NOP();

    SET_EN;                 //产生使能脉冲信号
    _NOP(); 
    _NOP(); 
    CLR_EN;		
}
/*******************************************
函数名称：WaitForEnable
功    能：等待1602液晶完成内部操作
参    数：无
返回值  ：无
********************************************/
void WaitForEnable(void) 
{
    P4DIR &= 0x00;  //将P4口切换为输入状态

    CLR_RS;
    SET_RW;
    _NOP();
    SET_EN; 
    _NOP();
    _NOP();
	
    while((P4IN & Busy)!=0);  //检测忙标志

    CLR_EN;

    P4DIR |= 0xFF;  //将P4口切换为输出状态
}	
                       
/*******************************************
函数名称：Delay5ms
功    能：延时约5ms
参    数：无
返回值  ：无
********************************************/
void Delay5ms(void)
{ 
    uint i=40000;
    while (i != 0)
    {
        i--;
    }
}
