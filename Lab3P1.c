//bmgbbr

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>
#include <sys/mman.h>
#include <wiringPi.h>


#define MY_PRIORITY 51

typedef struct{
	struct timespec Period;
	struct timespec InitTime;
	struct timespec next_period;
	}time_struct;
/*
static void inc_period(time_struct *pinfo)//adds the period to the current time
{
	pinfo->next_period.tv_nsec += pinfo->Period.tv_nsec;
 
	while (pinfo->next_period.tv_nsec >= 15000000000) 
	{
		pinfo->next_period.tv_sec++;//timespec nsec overflow 
   	    	pinfo->next_period.tv_nsec -= 15000000000;
  	}
}
*/
int wiringPiSetupGpio();
void pinMode (int pin, int mode) ;
void pullUpDnControl (int pin, int pud) ;
void digitalWrite (int pin, int value) ;
int digitalRead (int pin) ;


int main () 
{
	wiringPiSetupGpio();

	pullUpDnControl(16, PUD_DOWN);
	//pulldown congig / set_sched
	struct sched_param param; //sets the priority above that of the kernel module 
	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);
	
	time_struct *timer;
	
	int fd=open("dev/mem/", O_RDWR|O_SYNC);
	printf("%d", fd);
	unsigned int *baseadd=mmap(NULL, 1000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
//	iowrite32(0x00001240, baseadd);//turn first 3 LED's to output, may need to change to something other than iowrite
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	//pinMode(5, OUTPUT);
	pinMode(16, INPUT);
	
	unsigned int *GPEDS=baseadd+0x40/sizeof(unsigned int);
	unsigned int *GPSET=baseadd+0x1C/sizeof(unsigned int);
	unsigned int *GPCLR=baseadd+0x28/sizeof(unsigned int);
	
	
	digitalWrite(2, LOW);
	digitalWrite(3, LOW);
	digitalWrite(4, LOW);
	//digitalWrite(5, LOW);
	
	while(1)
	{
//		Light_1 ON | sleep | Light_1 OFF
//		iowrite32(0x00000004, GPSET);//turn on LED 1
		digitalWrite(2, HIGH);
		delay(4000);
//		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timer->next_period, NULL);//sleep until specified time on period
//		iowrite32(0x00000004, GPCLR);//turn off LED 1
		digitalWrite(2, LOW);
		delay(1500);
//		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timer->next_period, NULL);//sleep until specified time on period
		
//		Light_2 ON | sleep | Light_2 OFF
//		iowrite32(0x00000008, GPSET);//turn on LED 2
		digitalWrite(3, HIGH);
		delay(4000);
//		iowrite32(0x00000008, GPCLR);//turn off LED 2
		digitalWrite(3, LOW);
		delay(1500);
		
		unsigned int push=digitalRead(16);
		if(push == 1)//if a pedestrian pushes the crosswalk button
		{
//			Light_3 ON | sleep | Light_3 OFF
//			iowrite32(0x00000010, GPSET);//turn on LED 3
			digitalWrite(4, HIGH);
			delay(4000);

//			iowrite32(0x00000010, GPCLR);//turn off LED 3
			digitalWrite(4, LOW);
			delay(1500);
			
//			CLR GPEDS
//			iowrite32(0x00010000, Event);//Clear Event Status Register for button 1
			//push=0;
			//GPEDS==
		}
	}
	
	return 0;
} 
