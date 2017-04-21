#ifndef SERVO_H_
#define SERVO_H_

void timer_1B_init(void);
void TIMER4A_Handler(void);
void TIMER4B_Handler(void);

void clock_timer_init(void);
void motor_init();
unsigned move_servo(unsigned);

void servo_init(void);

#endif /* SERVO_H_ */
