/**
 *		@file servo.h
 *		@brief this header file will contain all the required
 *		definitions and basic servo functions
 *
 *		@author Team Drop Tables
 *
 *		@date April 26, 2017
 */

#ifndef SERVO_H_
#define SERVO_H_

void timer_1B_init(void);

void motor_init();
unsigned move_servo(unsigned);
void servo_init(void);

#endif /* SERVO_H_ */
