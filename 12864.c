#include  <msp430x14x.h>

#define uint unsigned int
#define uchar unsigned char

uchar shuzi_table[] = {"0123456789"};

#define LCD_DataIn    P4DIR=0x00    //数据口方向设置为输入
#define LCD_DataOut   P4DIR=0xff    //数据口方向设置为输出
#define LCD2MCU_Data  P4IN
#define MCU2LCD_Data  P4OUT
#define LCD_CMDOut    P3DIR|=0x07     //P3口的低三位设置为输出
#define LCD_RS_H      P3OUT|=BIT0      //P3.0
#define LCD_RS_L      P3OUT&=~BIT0     //P3.0
#define LCD_RW_H      P3OUT|=BIT1      //P3.1
#define LCD_RW_L      P3OUT&=~BIT1     //P3.1
#define LCD_EN_H      P3OUT|=BIT2      //P3.2
#define LCD_EN_L      P3OUT&=~BIT2     //P3.2

/*******************************************
函数名称：Delay_1ms
功    能：延时约1ms的时间
参    数：无
返回值  ：无
********************************************/
void Delay_1ms(void)
{
    uint i,j;
    for(j=640;j>0;j--)
      for(i=3;i>0;i--);
} 
/*******************************************
函数名称：Delay_Nms
功    能：延时N个1ms的时间
参    数：n--延时长度
返回值  ：无
********************************************/
void Delay_Nms(uint n)
{
    uint i;
    
    for(i = n;i > 0;i--)    
        Delay_1ms();
}
/*******************************************
函数名称：Write_Cmd
功    能：向液晶中写控制命令
参    数：cmd--控制命令
返回值  ：无
********************************************/
void Write_Cmd(uchar cmd)
{
    uchar lcdtemp = 0;
			
    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;  
    do                       //判忙
    {    
        LCD_EN_H;
       	_NOP();					
       	lcdtemp = LCD2MCU_Data; 
       	LCD_EN_L;
        
    }
    while(lcdtemp & 0x80); 
    
    LCD_DataOut;    
    LCD_RW_L;  		
    MCU2LCD_Data = cmd; 
    LCD_EN_H;
    _NOP();						  			
    LCD_EN_L;
}
/*******************************************
函数名称：Write_Data
功    能：向液晶中写显示数据
参    数：dat--显示数据
返回值  ：无
********************************************/
void  Write_Data(uchar dat)
{
    uchar lcdtemp = 0;   
        
    LCD_RS_L;
    LCD_RW_H;  
    LCD_DataIn;   
    do                       //判忙
    {    
        LCD_EN_H;
        _NOP();						
        lcdtemp = LCD2MCU_Data; 
        LCD_EN_L;      
    }
    while(lcdtemp & 0x80);  
    
    LCD_DataOut; 
    LCD_RS_H;
    LCD_RW_L;  
        
    MCU2LCD_Data = dat;
    LCD_EN_H;
    _NOP();
    LCD_EN_L;
} 


/*******************************************
函数名称：Read_Data
功    能：从液晶中读显示数据
参    数：
返回值  ：一个字节
********************************************/
uchar Read_Data()
{
    uchar temp;
    LCD_DataOut;//release data wire
    MCU2LCD_Data |= 0xff;
    LCD_RS_H;
    LCD_RW_H;
    LCD_DataIn;
    LCD_EN_H;
    Delay_1ms();
    
    temp=LCD2MCU_Data;
    Delay_1ms();
    LCD_EN_L;
    return temp;    
}

/*******************************************
函数名称：Ini_Lcd
功    能：初始化液晶模块
参    数：无
返回值  ：无
********************************************/
void Ini_Lcd(void)
{                  
    LCD_CMDOut;    //液晶控制端口设置为输出
    
    Delay_Nms(500);
    Write_Cmd(0x30);   //基本指令集
    Delay_1ms();
    Write_Cmd(0x02);   // 地址归位
    Delay_1ms();
    Write_Cmd(0x0c);   //整体显示打开,游标关闭
    Delay_1ms();
    Write_Cmd(0x01);   //清除显示
    Delay_1ms();
    Write_Cmd(0x06);   //游标右移
    Delay_1ms();
    Write_Cmd(0x80);   //设定显示的起始地址
}
/*******************************************
函数名称：Disp_HZ
功    能：控制液晶显示汉字
参    数：addr--显示位置的首地址
          pt--指向显示数据的指针
          num--显示字符个数
返回值  ：无
********************************************/
void Disp_HZ(uchar addr,const uchar * pt,uchar num)
{
    uchar i;
		
    Write_Cmd(addr); 
    for(i = 0;i < (num*2);i++) 
    Write_Data(*(pt++)); 
} 

void Disp_String(unsigned char x,unsigned char y,unsigned char *Str)
{  
    uchar pos;
    if((y>4)||(x>8)) 
        return;
    if(x==1){x=0x80;}
    else if(x==2){x=0x90;}
        else if(x==3){x=0x88;}
    else if(x==4){x=0x98;}
    pos=x+y;
    Write_Cmd(pos);

    while(*Str>0)
    { Write_Data(*Str); Str++;}
}

/*******************************************
函数名称:Clear_GDRAM
功    能:清除液晶GDRAM中的随机数据
参    数:无
返回值  :无
********************************************/
void Clear_GDRAM(void)
{
    uchar i,j,k;
    
    Write_Cmd(0x34);        //打开扩展指令集
    i = 0x80;            
    for(j = 0;j < 32;j++)
    {
    Write_Cmd(i++);
    Write_Cmd(0x80);
        for(k = 0;k < 16;k++)
        {
            Write_Data(0x00);
        }
    }
    i = 0x80;
    for(j = 0;j < 32;j++)
    {
        Write_Cmd(i++);
        Write_Cmd(0x88);	   
        for(k = 0;k < 16;k++)
        {
            Write_Data(0x00);
        } 
    }   
    Write_Cmd(0x30);        //回到基本指令集
}
/*******************************************
函数名称:Draw_PM
功    能:在整个液晶屏幕上画图
参    数:无
返回值  :无
********************************************/
void Draw_PM(const uchar *ptr)
{
    uchar i,j,k;
    
    Write_Cmd(0x34);        //打开扩展指令集
    i = 0x80;            
    for(j = 0;j < 32;j++)
    {
        Write_Cmd(i++);
        Write_Cmd(0x80);
        for(k = 0;k < 16;k++)
        {
            Write_Data(*ptr++);
        }
    }
    i = 0x80;
    for(j = 0;j < 32;j++)
    {
        Write_Cmd(i++);
        Write_Cmd(0x88);	   
        for(k = 0;k < 16;k++)
        {
            Write_Data(*ptr++);
        } 
    }  
    Write_Cmd(0x36);        //打开绘图显示
    Write_Cmd(0x30);        //回到基本指令集
}
/*******************************************
函数名称：Draw_TX
功    能：显示一个16*16大小的图形
参    数：Yaddr--Y地址
          Xaddr--X地址
          dp--指向图形数据存放地址的指针
返回值  ：无
********************************************/
void Draw_TX(uchar Yaddr,uchar Xaddr,const uchar * dp) 
{
    uchar j;
    uchar k=0;
 
    Write_Cmd(0x01);  //清屏,只能清除DDRAM
    Write_Cmd(0x34);  //使用扩展指令集，关闭绘图显示
    for(j=0;j<16;j++)
    {
        Write_Cmd(Yaddr++);       //Y地址
        Write_Cmd(Xaddr);    //X地址
        Write_Data(dp[k++]);  
        Write_Data(dp[k++]);
    }
    Write_Cmd(0x36);  //打开绘图显示
    Write_Cmd(0x30);  //回到基本指令集模式
}

//********************************************//
//函数名称：Draw_Point
//功    能：给定坐标并打点
//参    数：x:坐标(0~127)
//          y:坐标(0~63)
//参    数：color=1，该点显示；color=0，该点清除；
//********************************************************//
void GUI_Point(uchar x,uchar y,uchar color)
{
    uchar x_Dyte,x_byte; //定义列地址的字节位，及在字节中的哪1 位
    uchar y_Dyte,y_byte; //定义为上下两个屏(取值为0，1)，行地址(取值为0~31)
    uchar GDRAM_hbit,GDRAM_lbit;
    Write_Cmd(0x36); //扩展指令命令;
  /***X,Y 坐标互换，即普通的X,Y 坐标***/
    x_Dyte=x/16; //计算在16 个字节中的哪一个
    x_byte=x%16; //计算在该字节中的哪一位
    y_Dyte=y/32; //0 为上半屏，1 为下半屏
    y_byte=y%32; //计算在0~31 当中的哪一行
    Write_Cmd(0x80+y_byte); //设定行地址(y 坐标),即是垂直地址
    Write_Cmd(0x80+x_Dyte+8*y_Dyte); //设定列地址(x 坐标)，并通过8*y_Dyte 选定上下屏，即是水平地址
    Read_Data(); //预读取数据
    GDRAM_hbit=Read_Data(); //读取当前显示高8 位数据
    GDRAM_lbit=Read_Data(); //读取当前显示低8 位数据
    //read operation will change AC, reset
    Write_Cmd(0x80+y_byte); //设定行地址(y 坐标)
    Write_Cmd(0x80+x_Dyte+8*y_Dyte); //设定列地址(x 坐标)，并通过8*y_Dyte 选定上下屏
    if(x_byte<8) //判断其在高8 位，还是在低8 位
    {
         if(color==1)
             Write_Data(GDRAM_hbit|(0x01<<(7-x_byte))); //置位GDRAM 区高8 位数据中相应的点
         else
             Write_Data(GDRAM_hbit&(~(0x01<<(7-x_byte)))); //清除GDRAM 区高8 位数据中相应的点
         
         Write_Data(GDRAM_lbit); //显示GDRAM 区低8 位数据
    }
    else
    {
         Write_Data(GDRAM_hbit);         //写高8位数据
         if(color==1)
             Write_Data(GDRAM_lbit|(0x01<<(15-x_byte))); //置位GDRAM 区高8 位数据中相应的点
         else
             Write_Data(GDRAM_lbit&(~(0x01<<(15-x_byte))));//清除GDRAM区高8位数据中相应的点
    }
    Write_Cmd(0x30); //恢复到基本指令集
}

//********************************************************
//填充GDRAM 数据：
//参数：dat 为填充的数据
//dat为0x00可以作为清屏作用
//********************************************************
void GUI_Fill_GDRAM(unsigned char dat)
{
    uchar i,j,k;
    
    uchar bGDRAMAddrX = 0x80; //GDRAM 水平地址
    uchar bGDRAMAddrY = 0x80; //GDRAM 垂直地址
    for(i=0;i<2;i++)//上半屏
    {
        for(j=0;j<32;j++)//半屏32行
            for(k=0;k<8;k++)//8列
            {
                Write_Cmd(0x34); //设置为8 位MPU 接口，扩充指令集,绘图模式关
                Write_Cmd(bGDRAMAddrY+j); //垂直地址Y
                Write_Cmd(bGDRAMAddrX+k); //水平地址X
                Write_Data(dat);//D15-D8
                Write_Data(dat);//D7-D0
            }
        bGDRAMAddrX = 0x88;//下半屏
    }
    Write_Cmd(0x36); //打开绘图模式
    Write_Cmd(0x30); //恢复基本指令集，关闭绘图模式
}

//***********************************************//
//******************画水平线*********************//
//***********************************************//
void LCD_Draw_Line_X( uchar X0, uchar X1, uchar Y, uchar Color )
{   
    uchar Temp ;
    if( X0 > X1 )
    {
        Temp = X1 ;
        X1 = X0 ;
        X0 = Temp ;
    }
    for( ; X0 <= X1 ; X0++ )
    GUI_Point( X0, Y, Color );    
}

//***********************************************//
//画垂直线  第x列  从第y0行画到第y1行  颜色 color
//***********************************************//
void LCD_Draw_Line_Y( uchar X, uchar Y0, uchar Y1, uchar Color )
{
    uchar Temp ;
    if( Y0 > Y1 )
    {
        Temp = Y1 ;
        Y1 = Y0 ;
        Y0 = Temp ;
    }
    for(; Y0 <= Y1 ; Y0++)
    GUI_Point( X, Y0, Color);
}

//***********************************************//
//函数功能：画斜线(x,y)到(end x,end y)
//***********************************************//
void LCD_Draw_Line( uchar StartX, uchar StartY, uchar EndX, uchar EndY, uchar Color )
{
    int distance;      /*根据屏幕大小改变变量类型(如改为int型)*/
    int delta_x, delta_y ;
    int incx, incy ;
    do
    {
        delta_x = EndX - StartX ;
        delta_y = EndY - StartY ;
        //delta x
        if( delta_x > 0 )
            incx = 1;        
        else 
        {
            if( delta_x == 0 )
                incx = 0;
            else
                incx = -1 ;
        }
        //delta y
        if( delta_y > 0 )
            incy = 1 ;
        else 
        {
            if(delta_y == 0 )
                incy = 0 ;
            else
                incy = -1 ;
        }
        delta_x = incx * delta_x ;//jue dui zhi    
        delta_y = incy * delta_y ;
    
        if( delta_x > delta_y )
        {
            distance = delta_x ;
            StartX+=incx;
            StartY+=incy;
        }
       else
       {
           distance = delta_y;
           StartX+=incx;
           StartY+=incy;
       }
       GUI_Point( StartX, StartY, Color ); 
    }
    while(distance!=0);
}

//***********************************************//
//函数功能：画  sin函数曲线
//***********************************************//
void fsin2()
{
    float x,y;
    uchar x1,y1;
    for(x=0;x<(4*3.14159);x+=0.1)
    {
        y=1*sin(3*x);
        x1=10*x;
        y1=31-(10*y+0.5);			  //对y值进行四舍五入
        GUI_Point(x1,y1,1);
    }
}  

//****************************************//
//        画圆
//*****************************************//
void GUI_Circle(uchar x0,uchar y0,uchar r,uchar color)
{  
    signed char a,b;
    signed char di;
    if(r>31 ||r==0) 
        return; //参数过滤，次液晶显示的最大圆半径为31
    a=0;
    b=r;
    di=3-2*r; //判断下个点位置的标志
    while(a<=b)
    {
        GUI_Point(x0-b,y0-a,color); //3
        GUI_Point(x0+b,y0-a,color); //0
        GUI_Point(x0-a,y0+b,color); //1
        GUI_Point(x0-b,y0-a,color); //7
        GUI_Point(x0-a,y0-b,color); //2
        GUI_Point(x0+b,y0+a,color); //4
        GUI_Point(x0+a,y0-b,color); //5
        GUI_Point(x0+a,y0+b,color); //6
        GUI_Point(x0-b,y0+a,color);
        a++;
      /***使用Bresenham 算法画圆**/
        if(di<0)
            di +=4*a+6;
        else
        {
            di +=10+4*(a-b);
            b--;
        }
        GUI_Point(x0+a,y0+b,color);
    }
}
//***********************************************//
//函数功能：画  任意角度的直线
//***********************************************//
void Draw_Any_Line( unsigned char StartX, unsigned char StartY, unsigned char EndX, unsigned char EndY, unsigned char Color )
{
    int t, distance;      
    int x = 0 , y = 0 , delta_x, delta_y ;
    signed char incx, incy ;

    delta_x = EndX - StartX ;
    delta_y = EndY - StartY ;

    if( delta_x > 0 )
        incx = 1;
    else 
    {
        if( delta_x == 0 )
        {
            LCD_Draw_Line_Y( StartX, StartY, EndY,Color );
            return ;
        }
        else
            incx = -1 ;
    }
    
    if( delta_y > 0 )
        incy = 1 ;
    else 
    {
        if(delta_y == 0 )
        {
            LCD_Draw_Line_X( StartX, EndX, StartY, Color );
            return ;
        }
        else
            incy = -1 ;
    }

    delta_x = abs( delta_x );   
    delta_y = abs( delta_y );
    
    if( delta_x > delta_y )
        distance = delta_x ;
    else
        distance = delta_y ;
    
    GUI_Point( StartX, StartY, Color );  
   
    for( t = 0 ; t <= distance+1  ; t++ )
    {
        GUI_Point( StartX, StartY, Color );
        x += delta_x ;
        y += delta_y ;
        if( x > distance )
        {
            x -= distance ;
            StartX += incx ;
        }
        if( y > distance )
        {
            y -= distance ;
            StartY += incy ;
        }
    }
}

