#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
int mydev_id;

static irqreturn_t button_isr(int irq, void *dev_id)
{


	return IRQ_HANDLED;
}


static int Startup(void)
{

	int dummy = 0;

	unsigned int *myEvent = ioremap(0x3F200000, 1024); // point to the GPSEL register

	iowrite32(0x00009240, myEvent); //writing to GPSEL to set all LED's to outputs

	unsigned int *myEvent0 = myEvent+1;

	iowrite32(0x00000000, myEvent0); //pins 16-19 bcm pushbutton's

	unsigned int *myEvent1 = myEvent+2;

	iowrite32(0x00000000, myEvent1); //pin 20 bcm pushbutton

	unsigned int *myEvent2 = myEvent+37; //point to the GPPUD0 register and (GPIO Pull-up/pull-down)

	//iowrite32(0x00000001, myEvent2); // writing to GPPUD0 to set control signal as 'Pull Down'

	unsigned int *myEvent3 = myEvent+38; // point to the GPPUDCLK0 register

	//udelay(100); // provide the required setup time for the control signal

	//iowrite32(0x001F0000, myEvent3); //assert clock on all 5 push buttons 

	//udelay(100);
//
	//iowrite32(0x00000000, myEvent2); //writing to GPPUD to remove the control signal (disable GPPUD0)

	//iowrite32(0x001F0000, myEvent3); //assert clock on all 5 push button (disable GPPUDCLK0)
//
	unsigned int *myEvent4 = myEvent+31; // point to the GPAREN0 register
	
	iowrite32(0x001F0000, myEvent4); // writing to GPAREN0 to enable the rising edge detection
		
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button Handler", &mydev_id);

	enable_irq(79);
	
	return 0;
}

static void End(void)
{
	unsigned int *myEvent = ioremap(0x3F200040, 1024); // point to the GPEDS register

	unsigned int *myEvent1 = myEvent+15; // point to the GPAREN0 register 

	iowrite32(0x001F0000, myEvent); // clearing GPEDS

	iowrite32(0x00000000, myEvent1); // disable GPAREN0

	free_irq(79, &mydev_id);
}

module_init(Startup);
module_exit(End);
