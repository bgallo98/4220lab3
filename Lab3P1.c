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

static void inc_period(time_struct *pinfo)//adds the period to the current time
{
	pinfo->next_period.tv_nsec += pinfo->Period.tv_nsec;
 
	while (pinfo->next_period.tv_nsec >= 15000000000) 
	{
		pinfo->next_period.tv_sec++;//timespec nsec overflow 
   	    	pinfo->next_period.tv_nsec -= 15000000000;
  	}
}

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
	
	int fd=open("/dev/mem", O_RDWR|O_SYNC);
	
	unsigned int *virtual = mmap(NULL, 1000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
	unsigned int *switch1 = virtual + 16; 

	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(16, INPUT);
	
	//unsigned int *GPEDS=virtual+0x40/sizeof(unsigned int);
	//unsigned int *GPSET=virtual+0x1C/sizeof(unsigned int);
	//unsigned int *GPCLR=virtual+0x28/sizeof(unsigned int);
	
	
	digitalWrite(2, LOW);
	digitalWrite(3, LOW);
	digitalWrite(4, LOW);

	while(1)
	{
	//Turn on Red LED
		digitalWrite(2, HIGH);
		delay(4000);
	//Turn off Red LED
		digitalWrite(2, LOW);
		delay(1500);
	//turn on Green LED	
		digitalWrite(3, HIGH);
		delay(4000);
	//turn off Green LED
		digitalWrite(3, LOW);
		delay(1500);
		
		if(*switch1 == 0x00010000)//if a pedestrian pushes the crosswalk button
		{

			printf("hello\n\n");
			//turn on LED 3
			digitalWrite(4, HIGH);
			delay(4000);

//			//turn off LED 3
			digitalWrite(4, LOW);
			delay(1500);
			
//			CLR GPEDS
//			//Clear Event Status Register for button 1
			*switch1 == 0x00010000; //run loop
		}
	}
	
	return 0;
} 
