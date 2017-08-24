/**
 *		@file movement.c
 *		@brief This defines all functionality related to the servi.
 *
 *		@author Team Drop Tables
 *
 *		@date April 26, 2017
 */

#include "servo.h"

#include "timer.h"
#include "lcd.h"
#include "button.h"
#include "serial_wifi.h"
#include "math.h"
#include "driverlib/interrupt.h"


/// Time servo will wait between movements
uint32_t servo_wait = 50; // wait time in ms


/// Initialize the timers
/**
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
void timer1B_init(void) {

	unsigned pulse_period = 320000;
	unsigned mid_width = 0;

	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1; // Turn on clock to TIMER1B
	TIMER1_CTL_R &= ~TIMER_CTL_TBEN; //disable TIMER1B
	TIMER1_CFG_R |= TIMER_CFG_16_BIT; //set to 16bit
	TIMER1_TBMR_R |= TIMER_TBMR_TBMR_PERIOD|TIMER_TBMR_TBAMS; //set to periodic and alternate mode
	TIMER1_TBPR_R = (pulse_period >> 16) & 0xFF; //set timer prescaler
	TIMER1_TBILR_R = pulse_period & 0xFFFF; //set period
	TIMER1_TBPMR_R = (296000 >> 16) & 0xFF; // set match prescaler
	TIMER1_TBMATCHR_R = (pulse_period - mid_width) & 0xFFFF; // set timer value for initial intpt

	// enable timer1b
	TIMER1_CTL_R |= TIMER_CTL_TBEN;
}

/// Initialize the servo motor
/**
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
void motor_init(){
	SYSCTL_RCGCGPIO_R |= 0x2;

	GPIO_PORTB_AFSEL_R |= BIT5;
	GPIO_PORTB_DEN_R |= BIT5;
	GPIO_PORTB_DIR_R |= BIT5;

	GPIO_PORTB_PCTL_R |= 0x700000;
}

/// Moves servo to a specified degree between 0 and 180 degrees.
/**
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
unsigned move_servo(unsigned degree){
	unsigned pulse_width; // pulse width in cycles
	pulse_width = 155.56*degree+7500;
	unsigned period_width = 320000;

	TIMER1_TBMATCHR_R = (period_width - pulse_width); // set pulse width

	timer_waitMillis(servo_wait);
	return pulse_width;
}

/// Initializes the servo components
/**
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
void servo_init(void) {
	motor_init();
	button_init();
	timer1B_init();
}
