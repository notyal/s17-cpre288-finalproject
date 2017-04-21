#include "servo.h"

#include "timer.h"
#include "lcd.h"
#include "button.h"
#include "serial_wifi.h"
#include "math.h"
#include "driverlib/interrupt.h"

#define TIMER4A_PRESCALER 245
#define TIMER4B_PRESCALER 49 //49 is 200ms

// CONFIGURATION //////////////////////////////////////////////////
uint32_t servo_wait = 50; // wait time in ms
///////////////////////////////////////////////////////////////////

volatile uint8_t button = 0;
volatile int8_t direction = 1;
volatile int16_t degrees = 0;
volatile uint8_t delta_degrees = 1;


/// Initialize the timers
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

void clock_timer_init(void) {
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R4; // Turn on clock to TIMER4

	//Configure the timer for input capture mode
	TIMER4_CTL_R &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN); //disable timerB to allow us to change settings
	TIMER4_CFG_R |= TIMER_CFG_16_BIT; //set to 16 bit timer

	TIMER4_TAMR_R = TIMER_TAMR_TAMR_PERIOD; //set for periodic mode, down count
	TIMER4_TBMR_R = TIMER_TBMR_TBMR_PERIOD; //set for periodic mode, down count

	TIMER4_TAPR_R = TIMER4A_PRESCALER - 1; // 1 sec clock
	TIMER4_TBPR_R = TIMER4B_PRESCALER - 1;  // 200 ms clock

	TIMER4_TAILR_R = (int)(16000000/TIMER4A_PRESCALER); // set the load value for the 1 second clock (with your prescaler, how many clock cycles happen in 1 second?)
	TIMER4_TBILR_R = (int)(16000000/(TIMER4B_PRESCALER * 5)); // set the load value for the 0.2 second clock

	//clear TIMER3B interrupt flags
	TIMER4_ICR_R = (TIMER_ICR_TATOCINT | TIMER_ICR_TBTOCINT); //clears TIMER4 time-out interrupt flags
	TIMER4_IMR_R |= (TIMER_IMR_TATOIM | TIMER_IMR_TBTOIM); //enable TIMER4(A&B) time-out interrupts

	//initialize local interrupts
	NVIC_EN2_R = 0xC0;
	//go to page 105 and find the corresponding interrupt numbers for TIMER4 A&B
	//then set those bits in the correct interrupt set EN register (p. 142)

	IntRegister(INT_TIMER4A, TIMER4A_Handler); //register TIMER4A interrupt handler
	IntRegister(INT_TIMER4B, TIMER4B_Handler); //register TIMER4B interrupt handler

	IntMasterEnable(); //intialize global interrupts

	TIMER4_CTL_R |= (TIMER_CTL_TAEN | TIMER_CTL_TBEN); //Enable TIMER4A & TIMER4B
}

void motor_init(){
	SYSCTL_RCGCGPIO_R |= 0x2;

	GPIO_PORTB_AFSEL_R |= BIT5;
	GPIO_PORTB_DEN_R |= BIT5;
	GPIO_PORTB_DIR_R |= BIT5;

	GPIO_PORTB_PCTL_R |= 0x700000;
}

void TIMER4A_Handler(void)
{
	TIMER4_ICR_R = TIMER_ICR_TATOCINT; //clear the TIMER4A timeout interrupt flag (page 754 of datasheet)
}

void TIMER4B_Handler(void)
{
	TIMER4_ICR_R = TIMER_ICR_TBTOCINT; //clear flag
	//check buttons
	button = button_getButton();

	switch (button) {
			case 4:
				direction *= -1;
				break;
			case 3:
				delta_degrees = 5;
				break;
			case 2:
				delta_degrees = 2.5;
				break;
			case 1:
				delta_degrees = 1;
				break;
		}

	if (button != 0 && button != 4) degrees += delta_degrees*direction;

	//Check Bounds.
	if (degrees < 0) degrees = 0;
	else if (degrees > 180) degrees = 180;

}

unsigned move_servo(unsigned degree){
	unsigned pulse_width; // pulse width in cycles
	pulse_width = 155.56*degree+7500;
	unsigned period_width = 320000;

	TIMER1_TBMATCHR_R = (period_width - pulse_width); // set pulse width

	timer_waitMillis(servo_wait);
	return pulse_width;
}


void servo_init(void) {
	clock_timer_init();
	motor_init();
	button_init();
	timer1B_init();
}
