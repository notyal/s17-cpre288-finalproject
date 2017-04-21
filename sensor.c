#include "sensor.h"

#include "math.h"
#include "ir.h"
#include "ping.h"
#include "servo.h"
#include "timer.h"
#include "button.h"
#include "serial_wifi.h"
#include "open_interface.h"
#include "driverlib/interrupt.h"
#include <stdbool.h>

oi_t *sdata;
uint16_t counter;

/*
 * sensor.c
 *
 *  Created on: Apr 17, 2017
 *      Author: dsbis
 */

void TIMER2A_Handler(){
	TIMER2_ICR_R = TIMER_ICR_TBTOCINT; //clear flag

	lcd_printf("%u", counter);
	counter++;

	if(sdata->cliffFrontLeft || sdata->cliffFrontRight || sdata->cliffFrontLeftSignal || sdata->cliffFrontRightSignal ||
	   sdata->cliffLeft|| sdata->cliffRight || sdata->cliffLeftSignal || sdata->cliffRightSignal)
	{

		//Stop the bot
		oi_setWheels(0,0);

		// Transmit data over UART
		char buf[80];
		sprintf(buf, "Cliff or line found. Please reroute.");
		uart_puts(buf);
	}



//	oi_free(sdata);
}


void init_line_detection(){
	sdata = oi_alloc();
	oi_init(sdata);

	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R2; // Turn on clock to TIMER2

	//Configure the timer for input capture mode
	TIMER2_CTL_R &= ~(TIMER_CTL_TAEN); //disable timerA to allow us to change settings
	TIMER2_CFG_R |= TIMER_CFG_16_BIT; //set to 16 bit timer

	TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD; //set for periodic mode, down count

	TIMER2_TAPR_R = TIMER2A_PRESCALER - 1;  // 200 ms clock

	TIMER2_TAILR_R = (int)(16000000/(TIMER2A_PRESCALER * 5)); // set the load value for the 0.2 second clock

	//clear TIMER4B interrupt flags
	TIMER2_ICR_R = TIMER_ICR_TATOCINT; //clears TIMER2 time-out interrupt flags
	TIMER2_IMR_R |= TIMER_IMR_TATOIM; //enable TIMER2(A) time-out interrupts

	//initialize local interrupts
	NVIC_EN2_R = 0x800000;

	IntRegister(INT_TIMER2A, TIMER2A_Handler); //register TIMER2A interrupt handler

	IntMasterEnable(); //intialize global interrupts

	TIMER2_CTL_R |= TIMER_CTL_TAEN; //Enable TIMER2A
}


