#include "stdio.h"
#include "string.h"
int calendar()
{
	printf("this is a test\n");
	printf("===============================================================================\n");
	printf("                         Welcome to Peppa's Calendar! \n");
	cale();
	printf("                       Thanks for using Peppa's Calendar! \n");
	printf("===============================================================================\n");
	printf("\n");
	return 0;
}

	//TTY *calendarTty=tty_table+4;
	#define N 7
		
	void cale()
	{
		int year, month, x, y;
		while (1)
		{
			while (1)
			{
				printf("Please input the year and month (devide with space, enter [q] to quit):  ");
				int argc2;
				char* argv2[1024];
				scanf("%s", argv2);
				if(*(argv2[0])=='q') break;
				year=char2int(argv2[0]);
				month=char2int(argv2[1]);
				rili(year,month);
			}
			printf("Are you sure to quit Calendar? [y] ");
			int argc2;
			char* argv2[1024];
			scanf("%s", argv2);
			if(*(argv2[0])==0||*(argv2[0])=='y') break;
		}
	}
	void print(int day,int tian)
	{
		int a[N][N],i,j,sum=1;
		for(i=0,j=0;j<7;j++)
		{
			if(j<day)
				printf("    ");
			else
			{
				a[i][j]=sum;
				printf("   %d",sum++);
			}
		}
		printf("\n");
		for(i=1;sum<=tian;i++)
		{
			for(j=0;sum<=tian&&j<7;j++)
			{
				a[i][j]=sum;
				if (sum<10)
				{
					printf("   %d", sum++);
				}
				else{
					printf("  %d",sum++);
				}
			}
			printf("\n");
		}
	}

	int duo(int year)
	{
		if(year%4==0&&year%100!=0||year%400==0)
			return 1;
		else
			return 0;
	}


	int rili(int year,int month)
	{
		int day,tian,preday,strday;
		printf("\n");
		printf("------   %dmonth   %dyear  ------\n\n",month,year);
		printf(" SUN MON TUE WED THU FRI SAT\n");
		switch(month)
		{
			case 1:
			tian=31;
			preday=0;
			break;
			case 2:
			tian=28;
			preday=31;
			break;
			case 3:
			tian=31;
			preday=59;
			break;
			case 4:
			tian=30;
			preday=90;
			break;
			case 5:
			tian=31;
			preday=120;
			break;
			case 6:
			tian=30;
			preday=151;
			break;
			case 7:
			tian=31;
			preday=181;
			break;
			case 8:
			tian=31;
			preday=212;
			break;
			case 9:
			tian=30;
			preday=243;
			break;
			case 10:
			tian=31;
			preday=273;
			break;
			case 11:
			tian=30;
			preday=304;
			break;
			default:
			tian=31;
			preday=334;
		}
		if(duo(year)&&month>2)
		preday++;
		if(duo(year)&&month==2)
		tian=29;
		day=((year-1)*365+(year-1)/4-(year-1)/100+(year-1)/400+preday+1)%7;
		print(day,tian);
		printf("\n");
	}


