//--------------------------------------
//����ʱʱ��Cx51Դ����
//BlueXIII May 2006
//--------------------------------------



#include <Reg52.h>
#define u8 unsigned char		//Ϊ��д����,����u8Ϊ�޷����ַ���
#define u16 unsigned int		//����u16Ϊ�޷�������

u8 table[11]={0xFC,0x0C,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6,0x00}	;		//�����洢LED����ܵ����0~9�Լ��ո�
u8 time[10]={2,2,2,2,2,2,2,2,2,2};		//�����洢ʱ��ÿһλ�����ݣ�����time[9]���ر�����24���Ƶ�Сʱ�����ڵ�����λ/��λ���������¼0~23��Сʱ���ݣ��ٽ���λ��ʮλת����time[4]��time[5]
u8 fixno[9]={0,0,0,0,0,0,0,0,0};

u8 counter1=0;		//����T0�ļ�����1
u8 counter2=0;		//����T0��ڦ����2
bit flag1=0;		//��־λ,ÿ500ms�任һ��,����������ʱ������˸������
u8 flag2=0;		//��־��,�����жϵ���ʱģʽ����ʱ��ģʽ,0ʱΪ����ʱģʽ,1Ϊʱ��ģʽ��2Ϊ����ģʽ
u8 key=0;		//����,0ʱ��ʾ�ް���,1ΪSET������,2ΪSWITCH,3ΪADD,4ΪTIME/CLOCK,5ΪFIX
u8 ledno=0;		//�����ж�����ģʽ�й�괦����һλ

u8 th0init=0xd8;
u8 tl0init=0xf0;

sbit p10=P1^0;
sbit p11=P1^1;
sbit p12=P1^2;
sbit p13=P1^3;
sbit p14=P1^4;
sbit p27=P2^7;
sbit p26=P2^6;

void timer0(void);		//��ʱ���ж�
void caltime(void);		//����ʱģʽ�����ڼ���ʱ��
void printled(void);		//��ӡLED
void win(void);		//ʱ�䵽ʱ������ף,��ӡHAPPY��ʹ����������
u8 getkey(void);		//ȡ�ð�������
void settime(void);		//ʱ������ģʽ
void printset(void);		//����ʱ������ģʽ�Ĵ�ӡ���
void clockinit(void);		//ʱ��ģʽ�ĳ�ʼ��
void clock(void);		//ʱ��ģʽ
void fix(void);		//����ģʽ
void printfix(void);			//����ģʽ�Ĵ�ӡ���

void main(void)
{
   	//u8 i;
		//for(i=0;i<=9;i++) table[i]='0'+i;table[10]='b';		//����������ʱ��
   	//for(i=0;i<=9;i++) time[i]=2;

	TMOD=0x01;	//����T0Ϊ��ʽ1��װ���ֵ,ʹ10ms�ж�һ��
	TH0=0xd8;
	TL0=0xf0;
	
	EA=1;		//���ж�
	ET0=1;
	TR0=1;

	SCON=0x00;		//���ô���Ϊ��ʽ0
	
	settime();		//���ʱ
	
	while(1)
	{
		key=getkey();		//����������һֱ��ⰴ��
		if(key==1) settime();
		if(key==4) fix();
		if(key==5) {flag2=!flag2;}
	}
}

void timer0(void) interrupt 1 using 1		//T0�жϷ������,ʹ�üĴ�����1
{

	TH0=th0init;		//�����װ
	TL0=tl0init;

	counter1++;
	counter2++;
	if(counter1==100)		//ÿ1����Ӧһ��
	{
	 	counter1=0;
	 	if(flag2==0) caltime();
		if(flag2==1) clock();

	}
	if(counter2==50)		//ÿ500ms flag1ȡ��,���ڴ�ӡ��˸���ַ�
	{
		counter2=0;
		flag1=!flag1;
		if(flag2==2) printset();
		if(flag2==3) printfix();
	}
		
}	

/*
���ڼ���ʱ��ĺ���,time[0]~time[8]�ֱ������洢ÿһλ������,�ɼ��ټ�����,��Сʱ�Ƚ��ر�,��time[9]����ʾ,��ͨ�������time[4]��time[5]
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
     					if(time[9]>0) {time[9]--;time[4]=time[9]%10;time[5]=time[9]/10;}		//time[9]��ȡֵ��0~23֮��,��time[9]�ɼ����time[4]��time[5]
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
        							else win();		//��ʱ�䵽ʱ����win������ף
		 					}
						}
	  				}
	 			}
			}
  		}
 	}
 printled();		//����ӡ��ʱ��
}

void printled(void)
{
	u8 i;
 	for(i=0;i<=8;i++)
 	{
 		p27=0;		//P2.7��������
		TI=0;		//TI�������0
  		SBUF=table[time[i]];		//table[]�ĵ�iλ��Ӧ��LED���i��ʾ
  		while(TI==0);		//�ȴ����
		p27=1;
	}
}


void delay(void)		//��ʱ����,���ڷ���
{u16 i;
 for(i=0;i<500;i++){}
}

u8 getkey(void)
{
	if(p10==0) {delay();while(p10==0);return(1);}		//����delay()�Է�����̧��ʱ���ؼ�ֵ
 	if(p11==0) {delay();while(p11==0);return(2);}
 	if(p12==0) {delay();while(p12==0);return(3);}
 	if(p13==0) {delay();while(p13==0);return(4);}
 	if(p14==0) {delay();while(p14==0);return(5);}
 	return(0);
}
 

/*
ʱ�����ú���,�����ǱȽϸ��ӵ�һ������,����SWITCH���л���ĳһλʱ,��Ҫʹ��һλ��˸.������time[4]��time[5]������{��Сʱ���ĸ�λ��ʮλ},��Ҫ�˼���һλ,��ʮλΪ2ʱ,��λ���ֻ��ȡ
��3.
*/
void settime(void)
{
	u8 tmp;
	tmp=flag2;
	flag2=2;
	delay();
	key=0;
	ledno=0;		//SEC1Ĭ��ѡ��
	while(1)
	{
	 	key=getkey();
	 	if(key==1) {flag2=tmp;break;}		//���ٰ�һ��SET��,�򷵻�
		if(key==2)		//��SWITCH����������ѭ���л����λ
		{	
			if(ledno<8) ledno++;
	   		else ledno=0;
			//printset();
   		}
	
	 
		if(key==3)		//��ADD��ʹ�������λ��ֵ��1
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

void printset(void)		//����ģʽ����˸���
{	
	u8 i;
	for(i=0;i<=8;i++)
	{
		p27=0;
		TI=0;
		if((flag1==1)&&(ledno==i)) SBUF=table[10];		//��flag1Ϊ1��ledno=iʱ��������ո�
		else SBUF=table[time[i]];		//�����������
		while(TI==0);
		p27=1;
	}
}


void win(void)		//��ף����
{
	u8 happy[9]={0x77,0xCF,0xCF,0xEF,0x6F,0x00,0x00,0x00,0x00};		//����LED��HAPPY	
	u8 i,j;
	while(1)
	{
		for(j=0;j<=500;j++){p26=!p26;for(j=0;j<=10;j++);}
		for(j=0;j<=1000;j++) p26=0;		//����
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

void clockinit(void)		//ʱ�ӳ���ĳ�ʼ��,�������λ����;,�ұ�6λ��ʾʱ��,��7λ��ʾ������
{
	time[7]=0;
	time[8]=0;
	if((time[6]=0)||(time[6]=8)||(time[6]=9)) time[6]=1;
}

void clock(void)		//ʱ���ӳ���,�뵹��ʱ�㷨����
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



void fix(void)		//�����ӳ���,�����ֵԽ��,��ʱԽ��,���9999
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
	 	if(key==1) {flag2=tmp;break;}		//SET���൱��ȡ��
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

void printfix(void)		//����ģʽ����˸���
{	
	u8 i;
	for(i=0;i<=8;i++)
	{
		p27=0;
		TI=0;
		if((flag1==1)&&(ledno==i)) SBUF=table[10];		//��flag1Ϊ1��ledno=iʱ��������ո�
		else SBUF=table[fixno[i]];		//�����������
		while(TI==0);
		p27=1;
	}
}
