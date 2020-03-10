//bmgbbr
//modified version of Lab3P1

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
#include <pthread.h>
#include <semaphore.h>


#define RED_PRIORITY 51
#define YELLOW_PRIORITY 51
#define BLUE_PRIORITY 51

sem_t sem;

void Red_Thread()
{
	//set scheduler
	struct sched_param param;
	param.sched_priority = RED_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	while(1)
	{
		sem_wait(&sem); //lock
	//Turn on Red LED
		digitalWrite(2, HIGH);
		delay(1000);
	//Turn off Red LED
		digitalWrite(2, LOW);

		sem_post(&sem); //unlock
		usleep(5);
	}
}

void Yellow_Thread()
{
	//set scheduler
	struct sched_param param;
	param.sched_priority = YELLOW_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	while(1)
	{
		sem_wait(&sem); //lock
	//Turn on Yellow LED
		digitalWrite(3, HIGH);
		delay(1000);
	//Turn off Yellow LED
		digitalWrite(3, LOW);

		sem_post(&sem); //unlock
		usleep(5);
	}
pthread_exit(NULL);
}

void Blue_Thread()
{
	//set scheduler
	struct sched_param param;
	param.sched_priority = BLUE_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	//create virtual memory and offset to get the GPEDS register
	int fd=open("/dev/mem", O_RDWR|O_SYNC);
	
	unsigned int *virtual = mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
	unsigned int *switch1 = virtual + 16; //address of GPEDS register

	*switch1 = 0x00010000;
	
	while(1)
	{
		if(*switch1 == 0x00010000)//if a pedestrian pushes the crosswalk button
		{
			sem_wait(&sem); //lock
			//turn on Blue LED
			digitalWrite(5, HIGH);
			delay(1000);
			//turn off Blue LED
			digitalWrite(5, LOW);
			//clear GPEDS
			*switch1 = 0x00010000;
			sem_post(&sem); //unlock
			usleep(5);
		}
	}

pthread_exit(NULL);
}

int main () 
{
	//use BCM values
	wiringPiSetupGpio();
	
	//apply pulldown on GPEDS register
	pullUpDnControl(16, PUD_DOWN);

	//create semephore
	sem_init(&sem, 0, 1);

	//define LED's and pushbutton's as I/O respectively
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(16, INPUT);
	
	digitalWrite(2, LOW);
	digitalWrite(3, LOW);
	digitalWrite(5, LOW);

	//initialize threads
	pthread_t red;
	pthread_t yellow;
	pthread_t blue;

	//turn on Red LED (traffic)
	pthread_create(&red, NULL, (void*)Red_Thread, NULL);	
	//turn on Yellow LED (traffic)
	pthread_create(&yellow, NULL, (void*)Yellow_Thread, NULL);
	//turn on Blue LED (pedestrian) 
	pthread_create(&blue, NULL, (void*)Blue_Thread, NULL);

	pthread_join(red, NULL);
	pthread_join(yellow, NULL);
	pthread_join(blue, NULL);
	
	return 0;
} 
