/**********************
myUsbUart 串口测试
型号:STC15F2K60S2 主频:11.0592MHz
************************/
#include<STC15F2K60S2.h>
#include<intrins.h>
#include  <math.h>    //Keil library  
#include  <stdio.h>   //Keil library	
#include  <INTRINS.H>
#define uchar unsigned char
#define uint unsigned int

/*---------宏定义---------*/
#define cstFocs      11059200L  //晶振频率
#define cstBaud1     9600       //波特率
#define cstKeyMaxNum 100        //按键抖动次数
#define   DataPort P0    //LCD1602数据端口
sbit	  SCL=P2^5;//P1^0;      //IIC时钟引脚定义
sbit 	  SDA=P2^6;//P1^1;      //IIC数据引脚定义
sbit 		SEL0=P2^0;
sbit 		SEL1=P2^1;
sbit 		SEL2=P2^2;
sbit		SEL3=P2^3;

sbit        Key1=P3^2;



#define	SlaveAddress   0xA6	  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                              //ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A
typedef unsigned char  BYTE;
typedef unsigned short WORD;
char segtable[]=
{
0x3f,
0x06,
0x5b,
0x4f,
0x66,
0x6d,
0x7d,
0x07,
0x7f,
0x6f,
0x77,
0x7c,
0x39,
0x5e,
0x79,
0x71,
0x40
};



char segtabletwo[]=
{
0xbf,
0x86,
0xdb,
0xcf,
0xe6,
0xed,
0xfd,
0x87,
0xff,
0xef
};

BYTE BUF[8];//接收数据缓存区      	      
unsigned char ge_z,ge_y,ge_x,shi_z,shi_y,shi_x,bai_z,bai_y,bai_x;//显示变量uchar 
int  dis_data;//数据显示变量
char flag=0;//表示正在执行
double x_angle=0,y_angle=0,z_angle=0,angle=0,sum_y=0,sum_x=0,sum_z;//角度变量
uint avg_x=0,avg_y=0,avg_z,fla=0;//加速度变量

uint offx=0,offy=0,offz=0;//校准清零

void delay(unsigned int k);//延迟函数
void Init_ADXL345(void);//初始化ADXL345

void conversion(int temp_data_x,int temp_data_y);//转换出显示需要的数据

void  Single_Write_ADXL345(uchar REG_Address,uchar REG_data);   //单个写入数据
uchar Single_Read_ADXL345(uchar REG_Address);                   //单个读取内部寄存器数据
void  Multiple_Read_ADXL345();                                  //连续的读取内部寄存器数据
//------------------------------------
void Delay5us(); //延时5微秒(STC90C52RC@12M)
void Delay5ms(); //延时5毫秒(STC90C52RC@12M)
void ADXL345_Start(); //起始信号
void ADXL345_Stop(); //停止信号
void ADXL345_SendACK(bit ack);//发送应答信号
bit  ADXL345_RecvACK();	//接收应答信号
void ADXL345_SendByte(BYTE dat);//向IIC总线发送一个字节数据
BYTE ADXL345_RecvByte();//从IIC总线接收一个字节数据
//延迟函数
void Delay1(int n)
{
	int x;
	while(n--)
	{
		x=60;
		while(x--);
	}
}


//***************转换出显示需要的数据**********************
void conversion(int temp_data_x,int temp_data_y)  
{  
		bai_x=temp_data_x/100;
    temp_data_x=temp_data_x%100;     //取余运算
    shi_x=temp_data_x/10;
    temp_data_x=temp_data_x%10;      //取余运算
    ge_x=temp_data_x; 	
	
    bai_y=temp_data_y/100;
    temp_data_y=temp_data_y%100;     //取余运算
    shi_y=temp_data_y/10;
    temp_data_y=temp_data_y%10;      //取余运算
    ge_y=temp_data_y; 	
}

/**********延迟函数*********************/
void delay(unsigned int k)	
{						
uint i,j;				
for(i=0;i<k;i++)
{			
for(j=0;j<121;j++)			
{;}}						
}

/**************************************
延时5微秒(STC90C52RC@12M)
不同的工作环境,需要调整此函数，注意时钟过快时需要修改
当改用1T的MCU时,请调整此延时函数
**************************************/
void Delay5us()
{

	    WORD n =2;//560

    while (n--);
}

/**************************************
延时5毫秒(STC90C52RC@12M)
不同的工作环境,需要调整此函数
当改用1T的MCU时,请调整此延时函数
**************************************/
void Delay5ms()
{
    WORD n =10;//560

    while (n--);
}

/**************************************
起始信号
**************************************/
void ADXL345_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 0;                    //产生下降沿
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
}

/**************************************
停止信号
**************************************/
void ADXL345_Stop()
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 1;                    //产生上升沿
    Delay5us();                 //延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void ADXL345_SendACK(bit ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
}

/**************************************
接收应答信号
**************************************/
bit ADXL345_RecvACK()
{
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    CY = SDA;                   //读应答信号
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时

    return CY;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void ADXL345_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    ADXL345_RecvACK();
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
BYTE ADXL345_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        dat |= SDA;             //读数据               
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    return dat;
}

//******单字节写入*******************************************

void Single_Write_ADXL345(uchar REG_Address,uchar REG_data)
{
    ADXL345_Start();                  //起始信号
    ADXL345_SendByte(SlaveAddress);   //发送设备地址+写信号
    ADXL345_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
    ADXL345_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    ADXL345_Stop();                   //发送停止信号
}

//********单字节读取*****************************************
uchar Single_Read_ADXL345(uchar REG_Address)
{  uchar REG_data;
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress);           //发送设备地址+写信号
    ADXL345_SendByte(REG_Address);                   //发送存储单元地址，从0开始	
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    REG_data=ADXL345_RecvByte();              //读出寄存器数据
	ADXL345_SendACK(1);   
	ADXL345_Stop();                           //停止信号
    return REG_data; 
}
//*********************************************************
//
//连续读出ADXL345内部加速度数据，地址范围0x32~0x37
//
//*********************************************************
void Multiple_read_ADXL345(void)
{   uchar i;
// 		flag=0;//表示正在执行IIC操作
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress);           //发送设备地址+写信号
    ADXL345_SendByte(0x32);                   //发送存储单元地址，从0x32开始	
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress+1);         //发送设备地址+读信号
		for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        BUF[i] = ADXL345_RecvByte();          //BUF[0]存储0x32地址中的数据
        if (i == 5)
        {
           ADXL345_SendACK(1);                //最后一个数据需要回NOACK
        }
        else
        {
          ADXL345_SendACK(0);                //回应ACK
       }
   }
    ADXL345_Stop();                          //停止信号

    Delay5ms();
}


//*****************************************************************

//初始化ADXL345，根据需要请参考pdf进行修改************************
void Init_ADXL345()
{
   Single_Write_ADXL345(0x31,0x0B);   //测量范围,正负16g，13位模式
   Single_Write_ADXL345(0x2C,0x08);   //速率设定为12.5 参考pdf13页
   Single_Write_ADXL345(0x2D,0x08);   //选择电源模式   参考pdf24页
//   Single_Write_ADXL345(0x2E,0x80);   //使能 DATA_READY 中断
//   Single_Write_ADXL345(0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
//   Single_Write_ADXL345(0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
//   Single_Write_ADXL345(0x20,0x05);   //Z 偏移量 根据测试传感器的状态写入pdf29页
}
//***********************************************************************
//显示x轴
void display_x()
{   

	double temp;
    dis_data=(BUF[1]<<8)+BUF[0];  //合成数据   
	if(dis_data<0){
	dis_data=-dis_data;
	}
    temp=(double)dis_data*3.9;  //计算数据和显示,查考ADXL345快速入门第4页
//      conversion(temp);          //转换出显示需要的数据
		x_angle=temp;			

}

//***********************************************************************
//显示y轴
void display_y()
{     double temp;
    dis_data=(BUF[3]<<8)+BUF[2];  //合成数据   
	if(dis_data<0){
	dis_data=-dis_data;
	}
    temp=(double)dis_data*3.9;  //计算数据和显示,查考ADXL345快速入门第4页
//     conversion(temp);          //转换出显示需要的数据
		y_angle=temp;
}

//***********************************************************************
//显示z轴
void display_z()
{     double temp;
    dis_data=(BUF[5]<<8)+BUF[4];    //合成数据   
	if(dis_data<0){
	dis_data=-dis_data;
	}
    temp=(double)dis_data*3.9;  //计算数据和显示,查考ADXL345快速入门第4页
//       conversion(temp);          //转换出显示需要的数据

	z_angle=temp;
}
//显示角度
void display_angle()
{  
	double tempflag_y,tempflag_x;
	fla++;

	if(fla>19)
	{
			avg_y=sum_y/20;
			avg_x=sum_x/20;
		
		    if(((offx-1)<=avg_x<=(offx+1))&&((offy-1)<=avg_y<=(offy+1)))
		    conversion(abs(avg_x-offx),abs(avg_y-offy));
			
			else
			conversion(abs(avg_x),abs(avg_y)) ;
		
			fla=0;
			sum_y=0;
			sum_x=0;
	}

	display_x();
	display_y();
	display_z();
	
 	x_angle=-x_angle;
	y_angle=-y_angle;
 

	  tempflag_x=(double)(asin((double)((x_angle/(sqrt(x_angle*x_angle+y_angle*y_angle+z_angle*z_angle)))))*180/3.14159265);  //X轴角度值
	  tempflag_y=(double)(asin((double)((y_angle/(sqrt(x_angle*x_angle+y_angle*y_angle+z_angle*z_angle)))))*180/3.14159265);  //Y轴角度值
	  if(tempflag_x<0)
	  tempflag_x=- tempflag_x;
	  if(tempflag_y<0)
	  tempflag_y=- tempflag_y;

	
		tempflag_y=tempflag_y*10;
		tempflag_x=tempflag_x*10;
	
		sum_y+=tempflag_y;  
		sum_x+=tempflag_x;

}


/*---------变量定义---------*/
uchar ucT100usTimes;

bit btT1msFlag;            //1ms的标志


/*收发显示数据相关*/
bit btUart1SendBusy = 0 ;
uchar ucDateTmp;               //传输数据暂存
uchar ucDateDigState;
uchar arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71}; //显示0-f

/*---------定时器T0中断处理函数--------*/
void T0_Process() interrupt 1
{
    TH0 = ( 65535 - 1000 ) / 256; //定时器初始值
    TL0 = ( 65535 - 1000 ) % 256;
    ucT100usTimes++;
    if( ucT100usTimes == 10 )        //中断10次对应1ms
    {
        ucT100usTimes = 0;
        btT1msFlag = 1;
    }
    ucDateDigState++;
    if( ucDateDigState == 2 )
        ucDateDigState = 0;
    P0 = 0;
    switch( ucDateDigState )
    {
        case 0:
            P2 = 0x00;  P0 = arrSegSelect[ucDateTmp / 16];  
						break;
        case 1:
            P2 = 0x01;  P0 = arrSegSelect[ucDateTmp % 16];  
						break;
    }
}

/*---------串口1初始化函数--------*/
void Uart1_Init( void )
{
    AUXR = 0X80; //辅助寄存器 此时定时器0的速度是传统的12倍，不分频
    SCON |= 0X50; //允许接收
    TL1 = ( 65536 - ( cstFocs / 4 / cstBaud1 ) );
    TH1 = ( 65536 - ( cstFocs / 4 / cstBaud1 ) ) >> 8;
    AUXR |= 0X40; //辅助寄存器 此时定时器1的速度是传统的12倍，不分频
    RI = 0;      //接收中断标志位
    TI = 0;      //发送中断标志位
    TR1 = 1;     //启动定时器1
    ES = 1;      //串口中断允许位
    EA = 1;      //总中断允许位
    PS = 1 ;    //串口1中断高优先级
}

/*---------发送数据函数--------*/
void SendData( unsigned char dat )
{
    while( btUart1SendBusy ); //发送单个字符给UART1以发送到PC机
    btUart1SendBusy = 1;
    SBUF = dat;
}

/*---------串口1中断处理函数--------*/
void Uart1_Process() interrupt 4 using 1
{
    if( RI )         //接受完数据后 RI自动制1
    {
        RI = 0;   ucDateTmp = SBUF;
    }
    if( TI )        //发送完数据后 RI自动制1
    {
        TI = 0;   btUart1SendBusy = 0;
    }
}

/*---------初始化函数--------*/
void Init()
{
    P3M0 = 0x00;
    P3M1 = 0x00;
    P2M0 = 0xff;
    P2M1 = 0x00;
    P0M0 = 0xff;
    P0M1 = 0x00;

    TMOD = 0x01; //定时器0，方式1
    ET0 = 1; //开启定时器中断
    TH0 = ( 65535 - 1000 ) / 256;
    TL0 = ( 65535 - 1000 ) % 256;
    TR0 = 1; //启动定时器

    Uart1_Init();   //外部中断：低优先级

    ucDateTmp = 0x00;
    btT1msFlag = 0;

}

/*---------主函数--------*/
//*********************************************************
//******主程序********
//*********************************************************
void main()
{ 
  uchar devid;
  delay(500);//上电延时		

    //推挽输出
	P0M1=0x00;
    P0M0=0xff;

	P2M1=0x00;
	P2M0=0x08;
	SEL3=0;
	

  //液晶初始化ADXL345
  Init_ADXL345();                 //初始化ADXL345
  devid=Single_Read_ADXL345(0X00);//读出的数据为0XE5,表示正确
	
    
	Init() ;
  while(1)//循环
  { 
 		Multiple_Read_ADXL345();  
		display_angle();
		if(Key1==0)
		{
		 delay(5);
		 if(Key1==0)
		  {
		    Multiple_Read_ADXL345();
		    display_angle(); 
		    offx=avg_x;
		    offy=avg_y;
		  }
		  while(!Key1)	;
		  
		  }
			if(ucDateTmp == 0xbf){
				SendData(bai_x) ;
				SendData(shi_x) ;
				SendData(bai_y) ;
				SendData(shi_y) ;
				ucDateTmp = 0x00 ;
			}
  }
}