//--------------------------------------
//倒计时时钟Cx51源代码
//BlueXIII May 2006
//--------------------------------------



#include <Reg52.h>
#define u8 unsigned char		//为书写方便,定义u8为无符号字符型
#define u16 unsigned int		//定义u16为无符号整型

u8 table[11]={0xFC,0x0C,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6,0x00}	;		//用来存储LED数码管的码表，0~9以及空格
u8 time[10]={2,2,2,2,2,2,2,2,2,2};		//用来存储时间每一位的数据，其中time[9]较特别，由于24进制的小时不便于单独进位/借位，故用其计录0~23的小时数据，再将个位和十位转出到time[4]和time[5]
u8 fixno[9]={0,0,0,0,0,0,0,0,0};

u8 counter1=0;		//用于T0的计数器1
u8 counter2=0;		//用于T0的讦数器2
bit flag1=0;		//标志位,每500ms变换一次,用于在设置时产生闪烁的数字
u8 flag2=0;		//标志们,用于判断倒计时模式还是时钟模式,0时为倒计时模式,1为时钟模式，2为设置模式
u8 key=0;		//按键,0时表示无按键,1为SET键按下,2为SWITCH,3为ADD,4为TIME/CLOCK,5为FIX
u8 ledno=0;		//用于判断设置模式中光标处于哪一位

u8 th0init=0xd8;
u8 tl0init=0xf0;

sbit p10=P1^0;
sbit p11=P1^1;
sbit p12=P1^2;
sbit p13=P1^3;
sbit p14=P1^4;
sbit p27=P2^7;
sbit p26=P2^6;

void timer0(void);		//计时器中断
void caltime(void);		//倒计时模式中用于计算时间
void printled(void);		//打印LED
void win(void);		//时间到时用于庆祝,打印HAPPY并使蜂鸣器发声
u8 getkey(void);		//取得按键输入
void settime(void);		//时间设置模式
void printset(void);		//用于时间设置模式的打印输出
void clockinit(void);		//时钟模式的初始化
void clock(void);		//时钟模式
void fix(void);		//工程模式
void printfix(void);			//工程模式的打印输出

void main(void)
{
   	//u8 i;
		//for(i=0;i<=9;i++) table[i]='0'+i;table[10]='b';		//保留，测试时用
   	//for(i=0;i<=9;i++) time[i]=2;

	TMOD=0x01;	//设置T0为方式1并装入初值,使10ms中断一次
	TH0=0xd8;
	TL0=0xf0;
	
	EA=1;		//开中断
	ET0=1;
	TR0=1;

	SCON=0x00;		//设置串口为方式0
	
	settime();		//设初时
	
	while(1)
	{
		key=getkey();		//在主程序中一直检测按键
		if(key==1) settime();
		if(key==4) fix();
		if(key==5) {flag2=!flag2;}
	}
}

void timer0(void) interrupt 1 using 1		//T0中断服务程序,使用寄存器组1
{

	TH0=th0init;		//软件重装
	TL0=tl0init;

	counter1++;
	counter2++;
	if(counter1==100)		//每1秒响应一次
	{
	 	counter1=0;
	 	if(flag2==0) caltime();
		if(flag2==1) clock();

	}
	if(counter2==50)		//每500ms flag1取反,用于打印闪烁的字符
	{
		counter2=0;
		flag1=!flag1;
		if(flag2==2) printset();
		if(flag2==3) printfix();
	}
		
}	

/*
用于计算时间的函数,time[0]~time[8]分别用来存储每一位的数据,可减少计算量,但小时比较特别,用time[9]来表示,再通过计算得time[4]和time[5]
*/
void caltime(void)		
{
	if(time[0]>0) time[0]--;
 	else
 	{
 		time[0]=9;
  		if(time[1]>0) time[1]--;
  		else
  		{
				time[1]=5;
   			if(time[2]>0) time[2]--;
   			else
   			{
   				time[2]=9;
    				if(time[3]>0) time[3]--;
    				else
    				{
    					time[3]=5;
     					if(time[9]>0) {time[9]--;time[4]=time[9]%10;time[5]=time[9]/10;}		//time[9]的取值在0~23之间,由time[9]可计算出time[4]和time[5]
     					else
     					{
     						time[9]=23;time[4]=time[9]%10;time[5]=time[9]/10;
							if(time[6]>0) time[6]--;
      						else
      						{
      							time[6]=9;
       							if(time[7]>0) time[7]--;
       							else
       							{
       								time[7]=9;
        							if(time[8]>0) time[8]--;
        							else win();		//当时间到时调用win函数庆祝
		 					}
						}
	  				}
	 			}
			}
  		}
 	}
 printled();		//最后打印出时间
}

void printled(void)
{
	u8 i;
 	for(i=0;i<=8;i++)
 	{
 		p27=0;		//P2.7用于锁存
		TI=0;		//TI需软件置0
  		SBUF=table[time[i]];		//table[]的第i位对应着LED码的i显示
  		while(TI==0);		//等待输出
		p27=1;
	}
}


void delay(void)		//延时函数,用于防抖
{u16 i;
 for(i=0;i<500;i++){}
}

u8 getkey(void)
{
	if(p10==0) {delay();while(p10==0);return(1);}		//加入delay()以防抖，抬起时返回键值
 	if(p11==0) {delay();while(p11==0);return(2);}
 	if(p12==0) {delay();while(p12==0);return(3);}
 	if(p13==0) {delay();while(p13==0);return(4);}
 	if(p14==0) {delay();while(p14==0);return(5);}
 	return(0);
}
 

/*
时间设置函数,反而是比较复杂的一个部分,当用SWITCH键切换到某一位时,需要使这一位闪烁.且其中time[4]和time[5]的设置{即小时数的个位和十位},都要顾及另一位,如十位为2时,个位最多只能取
到3.
*/
void settime(void)
{
	u8 tmp;
	tmp=flag2;
	flag2=2;
	delay();
	key=0;
	ledno=0;		//SEC1默认选中
	while(1)
	{
	 	key=getkey();
	 	if(key==1) {flag2=tmp;break;}		//若再按一次SET键,则返回
		if(key==2)		//按SWITCH键由右至左循环切换光标位
		{	
			if(ledno<8) ledno++;
	   		else ledno=0;
			//printset();
   		}
	
	 
		if(key==3)		//按ADD键使光标所在位的值加1
		{
			
		 	switch(ledno)
		 	{
		 		case 0:
					if(time[0]<9) time[0]++;
					else time[0]=0;
					break;
		  		case 1:
					if(time[1]<5) time[1]++;
					else time[1]=0;
	 				break;
		 		case 2:
					if(time[2]<9) time[2]++;
					else time[2]=0;
					break;
	 	   		case 3:
					if(time[3]<5) time[3]++;
					else time[3]=0;
	 				break;
				case 4:
			   		if((time[5]==0)||(time[5]==1))
					{
						if(time[4]<9) time[4]++;
						else time[4]=0;
					}
					if(time[5]==2)
					{
						if(time[4]<3) time[0]++;
						else time[4]=0;
					}
					break;
				case 5:
      					if(time[4]<=3)
					{
						if(time[5]<2) time[5]++;
						else time[5]=0;
					}
					if(time[5]>3)
					{
						if(time[5]<1) time[5]++;
						else time[5]=0;
					}
 					break;
		   		case 6:
					if(time[6]<9) time[6]++;
					else time[6]=0;
					break;
	  			case 7:
        	   			if(time[7]<9) time[7]++;
					else time[7]=0;
	 				break;
		   		case 8:
					if(time[8]<9) time[8]++;
					else time[8]=0;
					break;
	   		}
			//printset();
		}
	}
}

void printset(void)		//设置模式的闪烁输出
{	
	u8 i;
	for(i=0;i<=8;i++)
	{
		p27=0;
		TI=0;
		if((flag1==1)&&(ledno==i)) SBUF=table[10];		//当flag1为1且ledno=i时正常输出空格
		else SBUF=table[time[i]];		//否则正常输出
		while(TI==0);
		p27=1;
	}
}


void win(void)		//庆祝函数
{
	u8 happy[9]={0x77,0xCF,0xCF,0xEF,0x6F,0x00,0x00,0x00,0x00};		//定义LED码HAPPY	
	u8 i,j;
	while(1)
	{
		for(j=0;j<=500;j++){p26=!p26;for(j=0;j<=10;j++);}
		for(j=0;j<=1000;j++) p26=0;		//发声
		if(flag1==0)
		for(i=0;i<=8;i++)
		{
			p27=0;
			TI=0;
			SBUF=happy[i];
			while(TI==0);
			p27=1;
		}
		else
		for(i=0;i<=8;i++)
		{
			p27=0;
			TI=0;
			SBUF=happy[8];
			while(TI==0);
			p27=1;
		}
	}
}

void clockinit(void)		//时钟程序的初始化,最左边两位无用途,右边6位显示时间,第7位显示星期数
{
	time[7]=0;
	time[8]=0;
	if((time[6]=0)||(time[6]=8)||(time[6]=9)) time[6]=1;
}

void clock(void)		//时钟子程序,与倒计时算法类似
{
	clockinit();
	if(time[0]<9) time[0]++;
	else
 	{
		time[0]=0;
  		if(time[1]<5) time[1]++;
  		else
  		{
			time[1]=0;
   			if(time[2]<9) time[2]++;
   			else
   			{
				time[2]=0;
    				if(time[3]<5) time[3]++;
    				else
    				{
					time[3]=0;
     					if(time[9]<23) {time[9]++;time[4]=time[9]%10;time[5]=time[9]/10;}
     					else
     					{
						time[9]=0;time[4]=time[9]%10;time[5]=time[9]/10;
      						if(time[6]<7) time[6]++;
      						else time[6]=1;
      
					}
	  			}
	 		}
		}
  	}

	printled();
}



void fix(void)		//修正子程序,输入的值越大,计时越快,最大9999
{
	
	u16 total=0;
	u16 tmp1;
	u16 tmp2;
	
	u8 tmp;
	tmp=flag2;
	flag2=3;

	delay();
	key=0;
	ledno=0;
	while(1)
	{
	 	key=getkey();
	 	if(key==1) {flag2=tmp;break;}		//SET键相当于取消
		if(key==2)
		{	
			if(ledno<3) ledno++;
	   	else ledno=0;
			//printfix();
   	}
	
	 
		if(key==3)
		{
		 	switch(ledno)
		 	{
		 		case 0:
					if(fixno[0]<9) fixno[0]++;
					else fixno[0]=0;
					break;
		  		case 1:
 	    			if(fixno[1]<9) fixno[1]++;
					else fixno[1]=0;
	 				break;
		 		case 2:
					if(fixno[2]<9) fixno[2]++;
					else fixno[2]=0;
					break;
	 	   		case 3:
       					if(fixno[3]<9) fixno[3]++;
					else fixno[3]=0;
	 				break;
			}
			//printfix();
		}
		if(key==4)
		{
	 		total=fixno[0]+10*fixno[1]+100*fixno[2]+1000*fixno[3]+50000;
	 		tmp1=total/256;
	 		tmp2=total%256;
	 		th0init=tmp1;
	 		tl0init=tmp2;
			flag2=tmp;
			break;
		}
	}
}

void printfix(void)		//设置模式的闪烁输出
{	
	u8 i;
	for(i=0;i<=8;i++)
	{
		p27=0;
		TI=0;
		if((flag1==1)&&(ledno==i)) SBUF=table[10];		//当flag1为1且ledno=i时正常输出空格
		else SBUF=table[fixno[i]];		//否则正常输出
		while(TI==0);
		p27=1;
	}
}
