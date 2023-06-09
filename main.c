#include <reg51.h>
#include <intrins.h>		//??_nop_( )?????????
#define uchar unsigned char
#define uint unsigned int

sbit led0=P1^0;//????LED?
sbit led1=P1^1;//????LED?
bit led;//???????

//???
sbit smg0=P2^2;	//?????
sbit smg1=P2^3;
sbit smg2=P2^4;								   
#define duan P0	//?????

//?????     0	   1      2    3      4    5     6     7     8     9     -
uchar table[]={~0xC0,~0xF9,~0xA4,~0xB0,~0x99,~0x92,~0x82,~0xF8,~0x80,~0x90,~0xBF};
//????? ?????0~9?-
uchar table_dot[]={0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef,~0xBF};

//???
//????11.0592MHZ/s
#define T0time 46070
	//	????46080	
	//	TH0=(65536-T0time)/256;
	//	TL0=(65536-T0time)%256;	
uchar count=0;	//??1? T0???????

//???
sbit BEEP=P1^5;

//??
//???????????
uchar whereNow;	//0:????;1:????;2:????;3:????;4:(??)???????
sbit K1=P3^1; 	//??????
sbit K2=P3^0;
sbit K3=P3^2;
sbit K4=P3^3;
//?????????? 
#define shanCount 128
uchar blankCount;//???? ????
uchar normalCount;//???? ????

//??
uchar shi=0,fen=0,miao=0;	//??????? ????

//??
uchar clockSetLocation;//????????? 0,???,?????????

//??
uchar alarmSetLocation;//????????????
uchar shi1=0,fen1=0,miao1=10;//??????? ???? 
bit alarmOnOff;//????????
bit alarmSetOrNot;//??????????

//??
uchar cldSetLocation;//????????????
uint year=2022;	//?? ? ????
uchar month=11,day=11;//?? ??? ????
#define minYear 2022
#define maxYear 2050
//?????????????
bit isLeapYear;	//??????? 1,?;0,??
bit cldSetOrNot;//??????????
uchar commonYearTable[]={0,31,28,31,30,31,30,31,31,30,31,30,31};//???????????
uchar leapYearTable[]={0,31,29,31,30,31,30,31,31,30,31,30,31};	//???????????
//????????????index[0]?,???????????index[month],????

//--------------------------------------------------------------------------------

void delay()	//???
{
	uchar i;
	for(i=0;i<128;i++);
}


void delay_10ms(uchar c)   //??????? 10ms
{
    uchar a,b;
    for(;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}

void smg(uchar wei,uchar number)//??????????
{
	switch(wei)//?????
	{
	case 7:smg2=1;smg1=1;smg0=1;break;
	case 6:smg2=1;smg1=1;smg0=0;break;
	case 5:smg2=1;smg1=0;smg0=1;break;
	case 4:smg2=1;smg1=0;smg0=0;break;
	case 3:smg2=0;smg1=1;smg0=1;break;
	case 2:smg2=0;smg1=1;smg0=0;break;
	case 1:smg2=0;smg1=0;smg0=1;break;
	case 0:smg2=0;smg1=0;smg0=0;break;
	}
	duan=number;//??
	delay();//???
	duan=0x00;//??
}

//void smg_display//????????
//(uchar duan7,uchar duan6,uchar duan5,uchar duan4,uchar duan3,uchar duan2,uchar duan1,uchar duan0)
//{
//smg(7,duan7);
//smg(6,duan6);
//smg(5,duan5);
//smg(4,duan4);
//smg(3,duan3);
//smg(2,duan2);
//smg(1,duan1);
//smg(0,duan0);
//}


void timer0_initial()//--------------------------???T0???
{
	TMOD=0x01;
	TH0=(65536-T0time)/256;//???11.0592MHZ
	TL0=(65536-T0time)%256;
	EA=1;
	ET0=1;
	TR0=1;
	TR1=0;
}


void is_leap_year()//--------------------------???????(???????????)
{
	if((year%4==0 && year%100!=0) || (year%400==0))
	isLeapYear=1;
	else
	isLeapYear=0;
}

void serviceTimer0() interrupt 1//-----------------------??????
{
	TH0=(65536-T0time)/256;//???????
	TL0=(65536-T0time)%256;
	count++;//???????+1
	if(20==count)//???????20?,?+1
	{
		miao++;
		count=0;//???????
	}
	if(60==miao)//?????60?,?+1
	{
		fen++;
		miao=0;//?????
	}
	if(60==fen)//?????60?,?+1
	{
		shi++;
		fen=0;//?????
	}
	if(24==shi)//?????24?,?+1
	{	 	
		day++;
		shi=0;//?????
	}	
	if(0==isLeapYear)//?????	
	{	
		if(day>commonYearTable[month])
		{
			 day=1;//???????????,???1
			 month++;
		}
	}else			//?????
	{
		if(day>leapYearTable[month])
		{
			 day=1;//???????????,???1
			 month++;
		}
	}		
	if(month>12)
	{
		month=1;//???????12,???1
		year++;
	}
	if(year>maxYear)
	{
		year=minYear;//???????????,?????????
	}
}


void clock_display()//-------------------------???????????
{
	//???? 
		//??????????????????0,??????????
		if(1==clockSetLocation&&blankCount>0)
		{
			smg(7,0x00);
			blankCount--;
		}
		//??????????????????0,??????????
		else if(1==clockSetLocation&&normalCount>0)
		{
			smg(7,table[shi/10]);
			normalCount--;
		}else if(1==clockSetLocation)
		{
			blankCount=shanCount;//???????
			normalCount=shanCount;	
		}else
		smg(7,table[shi/10]);

	//???? 
		//??????????????????0,??????????
		if(1==clockSetLocation&&blankCount>0)
		{
			smg(6,0x00);
			blankCount--;
		}
		//??????????????????0,??????????
		else if(1==clockSetLocation&&normalCount>0)
		{
			smg(6,table[shi%10]);
			normalCount--;
		}else if(1==clockSetLocation)
		{
			blankCount=shanCount;//???????
			normalCount=shanCount;	
		}else
		smg(6,table[shi%10]);

	//-	
		smg(5,table[10]);

	//???? 
		//??????????????????0,??????????
		if(2==clockSetLocation&&blankCount>0)
		{
			smg(4,0x00);
			blankCount--;
		}
		//??????????????????0,??????????
		else if(2==clockSetLocation&&normalCount>0)
		{
			smg(4,table[fen/10]);
			normalCount--;
		}else if(2==clockSetLocation)
		{
			blankCount=shanCount;//???????
			normalCount=shanCount;	
		}else
		smg(4,table[fen/10]);

	//???? 
		//??????????????????0,??????????
		if(2==clockSetLocation&&blankCount>0)
		{
			smg(3,0x00);
			blankCount--;
		}
		//??????????????????0,??????????
		else if(2==clockSetLocation&&normalCount>0)
		{
			smg(3,table[fen%10]);
			normalCount--;
		}else if(2==clockSetLocation)
		{
			blankCount=shanCount;//???????
			normalCount=shanCount;	
		}else
		smg(3,table[fen%10]);

	//-	
		smg(2,table[10]);

	//???? 
		//??????????????????0,??????????
		if(3==clockSetLocation&&blankCount>0)
		{
			smg(1,0x00);
			blankCount--;
		}
		//??????????????????0,??????????
		else if(3==clockSetLocation&&normalCount>0)
		{
			smg(1,table[miao/10]);
			normalCount--;
		}else if(3==clockSetLocation)
		{
			blankCount=shanCount;//???????
			normalCount=shanCount;	
		}else
		smg(1,table[miao/10]);

	//???? 
		//??????????????????0,??????????
		if(3==clockSetLocation&&blankCount>0)
		{
			smg(0,0x00);
			blankCount--;
		}
		//??????????????????0,??????????
		else if(3==clockSetLocation&&normalCount>0)
		{
			smg(0,table[miao%10]);
			normalCount--;
		}else if(3==clockSetLocation)
		{
			blankCount=shanCount;//???????
			normalCount=shanCount;	
		}else
		smg(0,table[miao%10]);
}


void clock_set()//--------------------------??(??????)
{
	if(0==K1 && 0==whereNow)	//?????????????????K1
	{
		delay_10ms(3);//??
		if(0==K1)//????????
		{	
			while(0==K1);//????
			delay_10ms(3);//??
			whereNow=1;
			clockSetLocation=1;
			TR0=0;
		}
	}
	if(1==whereNow)	//??????,4??????????????
	{
		if(0==K1)//????K1:????
		{
			delay_10ms(3);//??
			if(0==K1)//????????
			{	 
				while(0==K1);//??????
				delay_10ms(3);//??
				clockSetLocation++;//????+1
				if(clockSetLocation>3)clockSetLocation=1;
			}
		}

			if(1==clockSetLocation)//??�?�??
			{
				if(0==K2)//??K2:??-1
				{
					delay_10ms(3);//??
					if(0==K2)//????????
					{	 
						while(0==K2);//??????
						delay_10ms(3);//??
						if(0==shi)shi=24;
						shi--;
					}	
				}
				if(0==K3)//??K3:??+1
				{
					delay_10ms(3);//??
					if(0==K3)//????????
					{	 
						while(0==K3);//??????
						delay_10ms(3);//??
						shi++;
						if(24==shi)shi=0;
					}	
				}
			}

			if(2==clockSetLocation)//??�?�??
			{
				if(0==K2)//??K2:??-1
				{
					delay_10ms(3);//??
					if(0==K2)//????????
					{	 
						while(0==K2);//??????
						delay_10ms(3);//??
						if(0==fen)fen=60;
						fen--;
					}	
				}
				if(0==K3)//??K3:??+1
				{
					delay_10ms(3);//??
					if(0==K3)//????????
					{	 
						while(0==K3);//??????
						delay_10ms(3);//??
						fen++;
						if(60==fen)fen=0;
					}	
				}
			}

			if(3==clockSetLocation)//??�?�??
			{
				if(0==K2)//??K2:??-1
				{
					delay_10ms(3);//??
					if(0==K2)//????????
					{	 
						while(0==K2);//??????
						delay_10ms(3);//??
						if(0==miao)miao=60;
						miao--;
					}	
				}
				if(0==K3)//??K3:??+1
				{
					delay_10ms(3);//??
					if(0==K3)//????????
					{	 
						while(0==K3);//??????
						delay_10ms(3);//??
						miao++;
						if(60==miao)miao=0;
					}	
				}
			}	
	 
	if(0==K4)//????K4:?????????,????????
   	{
		delay_10ms(3);//??
		if(0==K4)//????????
		{	 
			while(0==K4);//??????
			delay_10ms(3);//??
			clockSetLocation=0;
			whereNow=0;
			TR0=1;
		}
	} 
	}
}




void alarm_display()//----------------------???????????
{
	if(0==alarmOnOff)//??????
		smg(7,0xf3);//'P.'??STOP,????
	if(1==alarmOnOff)//??????
		smg(7,0xf9);//'E.'??OPEN,???
	

//????
	//??????????????????0,??????????
	if(1==alarmSetLocation&&blankCount>0)
	{
		smg(5,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(1==alarmSetLocation&&normalCount>0)
	{
		smg(5,table[shi1/10]);
		normalCount--;
	}else if(1==alarmSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(5,table[shi1/10]);
//????
	//??????????????????0,??????????
	if(1==alarmSetLocation&&blankCount>0)
	{
		smg(4,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(1==alarmSetLocation&&normalCount>0)
	{
		smg(4,table_dot[shi1%10]);
		normalCount--;
	}else if(1==alarmSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(4,table_dot[shi1%10]);
//????
	//??????????????????0,??????????
	if(2==alarmSetLocation&&blankCount>0)
	{
		smg(3,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(2==alarmSetLocation&&normalCount>0)
	{
		smg(3,table[fen1/10]);
		normalCount--;
	}else if(2==alarmSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(3,table[fen1/10]);
//????
	//??????????????????0,??????????
	if(2==alarmSetLocation&&blankCount>0)
	{
		smg(2,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(2==alarmSetLocation&&normalCount>0)
	{
		smg(2,table_dot[fen1%10]);
		normalCount--;
	}else if(2==alarmSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(2,table_dot[fen1%10]);
//????
	//??????????????????0,??????????
	if(3==alarmSetLocation&&blankCount>0)
	{
		smg(1,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(3==alarmSetLocation&&normalCount>0)
	{
		smg(1,table[miao1/10]);
		normalCount--;
	}else if(3==alarmSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(1,table[miao1/10]);
//????
	//??????????????????0,??????????
	if(3==alarmSetLocation&&blankCount>0)
	{
		smg(0,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(3==alarmSetLocation&&normalCount>0)
	{
		smg(0,table_dot[miao1%10]);
		normalCount--;
	}else if(3==alarmSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(0,table_dot[miao1%10]);
}


void alarm_set()//----------------------????
{
	if(0==whereNow && 0==K2)//?????????????????K2
	{
		delay_10ms(3);//??
		if(0==K2)//????????
		{	
			while(0==K2);//????
			delay_10ms(3);//??
			whereNow=2;
		}
	}
	
	if(2==whereNow)//?????????,?????????????????
	{
		if(0==K1)//????K1:????
		{
			delay_10ms(3);//??
			if(0==K1)//????????
			{	 
				while(0==K1);//??????
				delay_10ms(3);//??
				alarmSetLocation++;//????+1
				if(4==alarmSetLocation)alarmOnOff=!alarmOnOff;
				if(alarmSetLocation>3)alarmSetLocation=0; 	
			}
		}		

			if(1==alarmSetLocation)//??�?�??
			{
				if(0==K2)//??K2:??-1
				{
					delay_10ms(3);//??
					if(0==K2)//????????
					{	 
						while(0==K2);//??????
						delay_10ms(3);//??
						if(0==shi1)shi1=24;
						shi1--;
					}	
				}
				if(0==K3)//??K3:??+1
				{
					delay_10ms(3);//??
					if(0==K3)//????????
					{	 
						while(0==K3);//??????
						delay_10ms(3);//??
						shi1++;
						if(24==shi1)shi1=0;
					}	
				}
			}

			if(2==alarmSetLocation)//??�?�??
			{
				if(0==K2)//??K2:??-1
				{
					delay_10ms(3);//??
					if(0==K2)//????????
					{	 
						while(0==K2);//??????
						delay_10ms(3);//??
						if(0==fen1)fen1=60;
						fen1--;
					}	
				}
				if(0==K3)//??K3:??+1
				{
					delay_10ms(3);//??
					if(0==K3)//????????
					{	 
						while(0==K3);//??????
						delay_10ms(3);//??
						fen1++;
						if(60==fen1)fen1=0;
					}	
				}
			}

			if(3==alarmSetLocation)//??�?�??
			{
				if(0==K2)//??K2:??-1
				{
					delay_10ms(3);//??
					if(0==K2)//????????
					{	 
						while(0==K2);//??????
						delay_10ms(3);//??
						if(0==miao1)miao1=60;
						miao1--;
					}	
				}
				if(0==K3)//??K3:??+1
				{
					delay_10ms(3);//??
					if(0==K3)//????????
					{	 
						while(0==K3);//??????
						delay_10ms(3);//??
						miao1++;
						if(60==miao1)miao1=0;
					}	
				}	
			}

			if(0==K4)//????K4:?????????,????????
		   	{
				delay_10ms(3);//??
				if(0==K4)//????????
				{	 
					while(0==K4);//??????
					delay_10ms(3);//??
					alarmSetLocation=0;
					whereNow=0;
				}
			} 
	}	
}

void alarm_exe()//---------------------------------------------????
//???????????????,??????:
//1???????=??????
//2?????????
//3??????????
//
//?:???????????????,?????
{	
	
	if( shi==shi1&&fen==fen1&&miao==miao1 && 1==alarmOnOff && 0==whereNow)
	{
		while(1==K1 && 1==K2 && 1==K3 && 1==K4)
		{
		BEEP=~BEEP;
		smg(7,0x77);//'A'
			BEEP=~BEEP;
		smg(6,0xb8);//'L.'
			BEEP=~BEEP;
		smg(5,table[shi1/10]);
			BEEP=~BEEP;
		smg(4,table_dot[shi1%10]);
			BEEP=~BEEP;
		smg(3,table[fen1/10]);
			BEEP=~BEEP;
		smg(2,table_dot[fen1%10]);
			BEEP=~BEEP;
		smg(1,table[miao1/10]);
			BEEP=~BEEP;
		smg(0,table[miao1%10]);
			BEEP=~BEEP;
		}
		while(0==K1 || 0==K2 || 0==K3 || 0==K4);//??,????????
		delay_10ms(3);
	}
}


void cld_display()//------------------------???????????
{
//????
	//??????????????????0,??????????
	if(1==cldSetLocation&&blankCount>0)
	{
		smg(7,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(1==cldSetLocation&&normalCount>0)
	{
		smg(7,table[year/1000]);
		normalCount--;
	}else if(1==cldSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(7,table[year/1000]);
//????
	//??????????????????0,??????????
	if(1==cldSetLocation&&blankCount>0)
	{
		smg(6,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(1==cldSetLocation&&normalCount>0)
	{
		smg(6,table[year/100%10]);
		normalCount--;
	}else if(1==cldSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(6,table[year/100%10]);
//????
	//??????????????????0,??????????
	if(1==cldSetLocation&&blankCount>0)
	{
		smg(5,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(1==cldSetLocation&&normalCount>0)
	{
		smg(5,table[year%100/10]);
		normalCount--;
	}else if(1==cldSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(5,table[year%100/10]);
//????
	//??????????????????0,??????????
	if(1==cldSetLocation&&blankCount>0)
	{
		smg(4,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(1==cldSetLocation&&normalCount>0)
	{
		smg(4,table_dot[year%10]);
		normalCount--;
	}else if(1==cldSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(4,table_dot[year%10]);
//????
	//??????????????????0,??????????
	if(2==cldSetLocation&&blankCount>0)
	{
		smg(3,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(2==cldSetLocation&&normalCount>0)
	{
		smg(3,table[month/10]);
		normalCount--;
	}else if(2==cldSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(3,table[month/10]);
//????
	//??????????????????0,??????????
	if(2==cldSetLocation&&blankCount>0)
	{
		smg(2,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(2==cldSetLocation&&normalCount>0)
	{
		smg(2,table_dot[month%10]);
		normalCount--;
	}else if(2==cldSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(2,table_dot[month%10]);
//????
	//??????????????????0,??????????
	if(3==cldSetLocation&&blankCount>0)
	{
		smg(1,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(3==cldSetLocation&&normalCount>0)
	{
		smg(1,table[day/10]);
		normalCount--;
	}else if(3==cldSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(1,table[day/10]);
//????
	//??????????????????0,??????????
	if(3==cldSetLocation&&blankCount>0)
	{
		smg(0,0x00);
		blankCount--;
	}
	//??????????????????0,??????????
	else if(3==cldSetLocation&&normalCount>0)
	{
		smg(0,table_dot[day%10]);
		normalCount--;
	}else if(3==cldSetLocation)
	{
		blankCount=shanCount;//???????
		normalCount=shanCount;	
	}else
	smg(0,table_dot[day%10]);	
}


void cld_set()//---------------------------????
{
	if(0==whereNow && 0==K3)//?????????????????K3
	{
		delay_10ms(3);//??
		if(0==K3)//????????
		{	
			while(0==K3);//????
			delay_10ms(3);//??
			whereNow=3;
		}
	}
	
	if(3==whereNow)//?????????,?????????????????
	{
		if(0==K1)//????K1:????
		{
			delay_10ms(3);//??
			if(0==K1)//????????
			{	 
				while(0==K1);//??????
				delay_10ms(3);//??
				cldSetLocation++;//????+1
				if(cldSetLocation>3)cldSetLocation=1; 	
			}
		}		

			if(1==cldSetLocation)//??�?�??
			{
				if(0==K2)//??K2:??-1
				{
					delay_10ms(3);//??
					if(0==K2)//????????
					{	 
						while(0==K2);//??????
						delay_10ms(3);//??
						if(minYear==year)year=minYear+1;//?????????????????
						year--;
					}	
				}
				if(0==K3)//??K3:??+1
				{
					delay_10ms(3);//??
					if(0==K3)//????????
					{	 
						while(0==K3);//??????
						delay_10ms(3);//??
						year++;
						if(maxYear+1==year)year=maxYear;//?????????????????
					}	
				}
			}

			if(2==cldSetLocation)//??�?�??
			{
				if(0==K2)//??K2:??-1
				{
					delay_10ms(3);//??
					if(0==K2)//????????
					{	 
						while(0==K2);//??????
						delay_10ms(3);//??
						if(1==month)month=13;
						month--;
					}	
				}
				if(0==K3)//??K3:??+1
				{
					delay_10ms(3);//??
					if(0==K3)//????????
					{	 
						while(0==K3);//??????
						delay_10ms(3);//??
						month++;
						if(13==month)month=1;
					}	
				}
			}

			if(3==cldSetLocation)//??�?�??
			{
				if(0==K2)//??K2:??-1
				{
					delay_10ms(3);//??
					if(0==K2)//????????
					{	 
						while(0==K2);//??????
						delay_10ms(3);//??
						if(day<=1)
						{					
							if(0==isLeapYear)//?????
							day=commonYearTable[month]+1;
							if(1==isLeapYear)//?????
							day=leapYearTable[month]+1;	
						}	
						day--;
					}	
				}
				if(0==K3)//??K3:??+1
				{
					delay_10ms(3);//??
					if(0==K3)//????????
					{	 
						while(0==K3);//??????
						delay_10ms(3);//??
						day++;					
						if(0==isLeapYear)//?????
						{
							if(day>commonYearTable[month])day=1;
						}
						if(1==isLeapYear)//?????
						{
							if(day>leapYearTable[month])day=1;
						}
					}	
				}	
			}

			if(0==K4)//????K4:?????????,????????
		   	{
				delay_10ms(3);//??
				if(0==K4)//????????
				{	 
					while(0==K4);//??????
					delay_10ms(3);//??
					cldSetLocation=0;
					whereNow=0;
				}
			} 
	}		
}

void led_display()//-----------------???????
{
	if(0==whereNow)
	{
		led0=1;
		led1=0;
	}
	else
	{
		led0=0;
		led1=1;
	}
}


void start_show()//------------------????
{
	uint t=1000;	
	while(t--)//20221111(????????)
	{
		smg(7,table[2]);
		smg(6,table[0]);
		smg(5,table[2]);
		smg(4,table[2]);
		smg(3,table[1]);
		smg(2,table[1]);
		smg(1,table[1]);
		smg(0,table[1]);
	}
	t=1000;//t??
	while(t--)//LHL ELEC.
	{
		smg(7,0x38);//L
		smg(6,0x76);//H
		smg(5,0x38);//L
		smg(4,0x00);//?
		smg(3,0x79);//E
		smg(2,0x38);//L
		smg(1,0x79);//E
		smg(0,0xb9);//C.
	}	
}

void main()
{
	start_show();	//????
	clockSetLocation=0;	//
	alarmSetOrNot=0;	//
	alarmSetLocation=0;	//
	cldSetOrNot=0;		//
	cldSetLocation=0;  	//??????????????????0
	alarmOnOff=0;		//??????
	whereNow=0;			//??????????
	blankCount=shanCount; //
	normalCount=shanCount;//?????????
	timer0_initial();	//???T0???
	led=led0;

	while(1)
	{
		is_leap_year();	//???????	
		if(0==whereNow || 1==whereNow)clock_display();//???????????
		clock_set();	//??(??????)	
		if(2==whereNow)alarm_display();	//???????????
		alarm_set();	//????
		alarm_exe();	//????
		if(3==whereNow)cld_display();//???????????
		cld_set();		//????
		//led_display();			//???????
	}
}0