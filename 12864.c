#include  <msp430x14x.h>

#define uint unsigned int
#define uchar unsigned char

uchar shuzi_table[] = {"0123456789"};

#define LCD_DataIn    P4DIR=0x00    //���ݿڷ�������Ϊ����
#define LCD_DataOut   P4DIR=0xff    //���ݿڷ�������Ϊ���
#define LCD2MCU_Data  P4IN
#define MCU2LCD_Data  P4OUT
#define LCD_CMDOut    P3DIR|=0x07     //P3�ڵĵ���λ����Ϊ���
#define LCD_RS_H      P3OUT|=BIT0      //P3.0
#define LCD_RS_L      P3OUT&=~BIT0     //P3.0
#define LCD_RW_H      P3OUT|=BIT1      //P3.1
#define LCD_RW_L      P3OUT&=~BIT1     //P3.1
#define LCD_EN_H      P3OUT|=BIT2      //P3.2
#define LCD_EN_L      P3OUT&=~BIT2     //P3.2

/*******************************************
�������ƣ�Delay_1ms
��    �ܣ���ʱԼ1ms��ʱ��
��    ������
����ֵ  ����
********************************************/
void Delay_1ms(void)
{
    uint i,j;
    for(j=640;j>0;j--)
      for(i=3;i>0;i--);
} 
/*******************************************
�������ƣ�Delay_Nms
��    �ܣ���ʱN��1ms��ʱ��
��    ����n--��ʱ����
����ֵ  ����
********************************************/
void Delay_Nms(uint n)
{
    uint i;
    
    for(i = n;i > 0;i--)    
        Delay_1ms();
}
/*******************************************
�������ƣ�Write_Cmd
��    �ܣ���Һ����д��������
��    ����cmd--��������
����ֵ  ����
********************************************/
void Write_Cmd(uchar cmd)
{
    uchar lcdtemp = 0;
			
    LCD_RS_L;
    LCD_RW_H;
    LCD_DataIn;  
    do                       //��æ
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
�������ƣ�Write_Data
��    �ܣ���Һ����д��ʾ����
��    ����dat--��ʾ����
����ֵ  ����
********************************************/
void  Write_Data(uchar dat)
{
    uchar lcdtemp = 0;   
        
    LCD_RS_L;
    LCD_RW_H;  
    LCD_DataIn;   
    do                       //��æ
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
�������ƣ�Read_Data
��    �ܣ���Һ���ж���ʾ����
��    ����
����ֵ  ��һ���ֽ�
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
�������ƣ�Ini_Lcd
��    �ܣ���ʼ��Һ��ģ��
��    ������
����ֵ  ����
********************************************/
void Ini_Lcd(void)
{                  
    LCD_CMDOut;    //Һ�����ƶ˿�����Ϊ���
    
    Delay_Nms(500);
    Write_Cmd(0x30);   //����ָ�
    Delay_1ms();
    Write_Cmd(0x02);   // ��ַ��λ
    Delay_1ms();
    Write_Cmd(0x0c);   //������ʾ��,�α�ر�
    Delay_1ms();
    Write_Cmd(0x01);   //�����ʾ
    Delay_1ms();
    Write_Cmd(0x06);   //�α�����
    Delay_1ms();
    Write_Cmd(0x80);   //�趨��ʾ����ʼ��ַ
}
/*******************************************
�������ƣ�Disp_HZ
��    �ܣ�����Һ����ʾ����
��    ����addr--��ʾλ�õ��׵�ַ
          pt--ָ����ʾ���ݵ�ָ��
          num--��ʾ�ַ�����
����ֵ  ����
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
��������:Clear_GDRAM
��    ��:���Һ��GDRAM�е��������
��    ��:��
����ֵ  :��
********************************************/
void Clear_GDRAM(void)
{
    uchar i,j,k;
    
    Write_Cmd(0x34);        //����չָ�
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
    Write_Cmd(0x30);        //�ص�����ָ�
}
/*******************************************
��������:Draw_PM
��    ��:������Һ����Ļ�ϻ�ͼ
��    ��:��
����ֵ  :��
********************************************/
void Draw_PM(const uchar *ptr)
{
    uchar i,j,k;
    
    Write_Cmd(0x34);        //����չָ�
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
    Write_Cmd(0x36);        //�򿪻�ͼ��ʾ
    Write_Cmd(0x30);        //�ص�����ָ�
}
/*******************************************
�������ƣ�Draw_TX
��    �ܣ���ʾһ��16*16��С��ͼ��
��    ����Yaddr--Y��ַ
          Xaddr--X��ַ
          dp--ָ��ͼ�����ݴ�ŵ�ַ��ָ��
����ֵ  ����
********************************************/
void Draw_TX(uchar Yaddr,uchar Xaddr,const uchar * dp) 
{
    uchar j;
    uchar k=0;
 
    Write_Cmd(0x01);  //����,ֻ�����DDRAM
    Write_Cmd(0x34);  //ʹ����չָ����رջ�ͼ��ʾ
    for(j=0;j<16;j++)
    {
        Write_Cmd(Yaddr++);       //Y��ַ
        Write_Cmd(Xaddr);    //X��ַ
        Write_Data(dp[k++]);  
        Write_Data(dp[k++]);
    }
    Write_Cmd(0x36);  //�򿪻�ͼ��ʾ
    Write_Cmd(0x30);  //�ص�����ָ�ģʽ
}

//********************************************//
//�������ƣ�Draw_Point
//��    �ܣ��������겢���
//��    ����x:����(0~127)
//          y:����(0~63)
//��    ����color=1���õ���ʾ��color=0���õ������
//********************************************************//
void GUI_Point(uchar x,uchar y,uchar color)
{
    uchar x_Dyte,x_byte; //�����е�ַ���ֽ�λ�������ֽ��е���1 λ
    uchar y_Dyte,y_byte; //����Ϊ����������(ȡֵΪ0��1)���е�ַ(ȡֵΪ0~31)
    uchar GDRAM_hbit,GDRAM_lbit;
    Write_Cmd(0x36); //��չָ������;
  /***X,Y ���껥��������ͨ��X,Y ����***/
    x_Dyte=x/16; //������16 ���ֽ��е���һ��
    x_byte=x%16; //�����ڸ��ֽ��е���һλ
    y_Dyte=y/32; //0 Ϊ�ϰ�����1 Ϊ�°���
    y_byte=y%32; //������0~31 ���е���һ��
    Write_Cmd(0x80+y_byte); //�趨�е�ַ(y ����),���Ǵ�ֱ��ַ
    Write_Cmd(0x80+x_Dyte+8*y_Dyte); //�趨�е�ַ(x ����)����ͨ��8*y_Dyte ѡ��������������ˮƽ��ַ
    Read_Data(); //Ԥ��ȡ����
    GDRAM_hbit=Read_Data(); //��ȡ��ǰ��ʾ��8 λ����
    GDRAM_lbit=Read_Data(); //��ȡ��ǰ��ʾ��8 λ����
    //read operation will change AC, reset
    Write_Cmd(0x80+y_byte); //�趨�е�ַ(y ����)
    Write_Cmd(0x80+x_Dyte+8*y_Dyte); //�趨�е�ַ(x ����)����ͨ��8*y_Dyte ѡ��������
    if(x_byte<8) //�ж����ڸ�8 λ�������ڵ�8 λ
    {
         if(color==1)
             Write_Data(GDRAM_hbit|(0x01<<(7-x_byte))); //��λGDRAM ����8 λ��������Ӧ�ĵ�
         else
             Write_Data(GDRAM_hbit&(~(0x01<<(7-x_byte)))); //���GDRAM ����8 λ��������Ӧ�ĵ�
         
         Write_Data(GDRAM_lbit); //��ʾGDRAM ����8 λ����
    }
    else
    {
         Write_Data(GDRAM_hbit);         //д��8λ����
         if(color==1)
             Write_Data(GDRAM_lbit|(0x01<<(15-x_byte))); //��λGDRAM ����8 λ��������Ӧ�ĵ�
         else
             Write_Data(GDRAM_lbit&(~(0x01<<(15-x_byte))));//���GDRAM����8λ��������Ӧ�ĵ�
    }
    Write_Cmd(0x30); //�ָ�������ָ�
}

//********************************************************
//���GDRAM ���ݣ�
//������dat Ϊ��������
//datΪ0x00������Ϊ��������
//********************************************************
void GUI_Fill_GDRAM(unsigned char dat)
{
    uchar i,j,k;
    
    uchar bGDRAMAddrX = 0x80; //GDRAM ˮƽ��ַ
    uchar bGDRAMAddrY = 0x80; //GDRAM ��ֱ��ַ
    for(i=0;i<2;i++)//�ϰ���
    {
        for(j=0;j<32;j++)//����32��
            for(k=0;k<8;k++)//8��
            {
                Write_Cmd(0x34); //����Ϊ8 λMPU �ӿڣ�����ָ�,��ͼģʽ��
                Write_Cmd(bGDRAMAddrY+j); //��ֱ��ַY
                Write_Cmd(bGDRAMAddrX+k); //ˮƽ��ַX
                Write_Data(dat);//D15-D8
                Write_Data(dat);//D7-D0
            }
        bGDRAMAddrX = 0x88;//�°���
    }
    Write_Cmd(0x36); //�򿪻�ͼģʽ
    Write_Cmd(0x30); //�ָ�����ָ����رջ�ͼģʽ
}

//***********************************************//
//******************��ˮƽ��*********************//
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
//����ֱ��  ��x��  �ӵ�y0�л�����y1��  ��ɫ color
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
//�������ܣ���б��(x,y)��(end x,end y)
//***********************************************//
void LCD_Draw_Line( uchar StartX, uchar StartY, uchar EndX, uchar EndY, uchar Color )
{
    int distance;      /*������Ļ��С�ı��������(���Ϊint��)*/
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
//�������ܣ���  sin��������
//***********************************************//
void fsin2()
{
    float x,y;
    uchar x1,y1;
    for(x=0;x<(4*3.14159);x+=0.1)
    {
        y=1*sin(3*x);
        x1=10*x;
        y1=31-(10*y+0.5);			  //��yֵ������������
        GUI_Point(x1,y1,1);
    }
}  

//****************************************//
//        ��Բ
//*****************************************//
void GUI_Circle(uchar x0,uchar y0,uchar r,uchar color)
{  
    signed char a,b;
    signed char di;
    if(r>31 ||r==0) 
        return; //�������ˣ���Һ����ʾ�����Բ�뾶Ϊ31
    a=0;
    b=r;
    di=3-2*r; //�ж��¸���λ�õı�־
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
      /***ʹ��Bresenham �㷨��Բ**/
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
//�������ܣ���  ����Ƕȵ�ֱ��
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

