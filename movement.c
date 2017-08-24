/**
 *		@file movement.c
 *		@brief This defines all functionality related to moving the bot.
 *
 *		@author Team Drop Tables
 *
 *		@date April 26, 2017
 */

#include "movement.h"
#include "open_interface.h"
#include "timer.h"
#include "serial_wifi.h"
#include <stdlib.h>
#include <stdint.h>


/// Allocates and initializes bot's sensor data.
/**
 *	 	Allocates and sets the bot's sensor data to be used across all functions.
 * 		Must be used first to initialize and use any movement function.
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 *
 */
void set_sensor_data(void){
	sdata = oi_alloc();
	oi_init(sdata);
}

/// Frees all memory used by the sensor data.
/**
 * 		Frees all memory used by the sensor data.
 * 		Must be used at the end of the program to avoid leaking memory.
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
void free_sensor_data(void){
	oi_free(sdata);
}

///Sets the bot's wheels to move forward a given amount.
/**
 * 		Sets the bot's wheels to move forward a given amount.
 * 		Override is used in order to block any test sensor from triggering a stop.
 * 		Returns the total distance traveled by the bot during a single call.
 * 		@param[in]  mm       The distance desired to travel in millimeters.
 * 		@param[in]  override An integer value used to skip hazard testing on individual sensors.
 * 		@param[out] dist 	The total distance traveled  before sensing a hazard.
 *
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
uint16_t move_forward(uint16_t mm, uint8_t override){
	uint16_t sum = 0;

	oi_setWheels(100, 100);

	while(sum < mm){
		oi_update(sdata);

		sum += sdata->distance;

		if(test_sensors(override)){
			move_backward(50);
			break;
		}
	}

	oi_setWheels(0,0);

	return sum;
}

/// Sets the bot's wheels to move backward a given amount.
/**
 * 		Sets the bot's wheels to move backward a given amount.
 * 		Returns the total distance traveled by the bot during a single call.
 * 		@param[in]  mm       The distance desired to travel in millimeters.
 * 		@param[out] dist 	The total distance traveled  before sensing a hazard.
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
uint8_t move_backward(uint8_t mm){
	int sum = 0;

	oi_setWheels(-50, -50);

	while(sum < mm){
		oi_update(sdata);

		sum += abs(sdata->distance);
	}

	oi_setWheels(0,0);

	return sum;
}

///Tests the bot's cliff sensors
/**
 * 		Tests the bot's cliff sensors
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
static int test_cliff(){

	if(sdata->cliffFrontLeft){
		return CLIFF_FL;
	}
	else if(sdata->cliffFrontRight){
		return CLIFF_FR;
	}
	else if(sdata->cliffLeft){
		return CLIFF_L;
	}
	else if(sdata->cliffRight){
		return CLIFF_R;
	}

	return 0;
}

///Tests the bot's line sensors
/**
 * 		Tests the bot's line sensors
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
static int test_line(){
	if(sdata->cliffFrontLeftSignal > LINE_DET){
		return LINE_FL;
	}
	else if(sdata->cliffFrontRightSignal > LINE_DET){
		return LINE_FR;
	}
	else if(sdata->cliffLeftSignal > LINE_DET){
		return LINE_L;
	}
	else if(sdata->cliffRightSignal > LINE_DET){
		return LINE_R;
	}

	return 0;
}

///Tests the bots's light sensors in the bumpers
/**
 * 		Tests the bots's light sensors in the bumpers
 * 		@param[out] detect Returns non-zero if light sensor is above specified bound
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
int test_lightbump(){

	if(sdata->lightBumpCenterLeftSignal > BUMP_DET){
		return LIGHTB_CL;
	}
	else if(sdata->lightBumpFrontLeftSignal > BUMP_DET){
		return LIGHTB_FL;
	}
	else if(sdata->lightBumpFrontRightSignal > BUMP_DET){
		return LIGHTB_FR;
	}
	else if(sdata->lightBumpCenterRightSignal > BUMP_DET){
		return LIGHTB_CR;
	}
	else if(sdata->lightBumpRightSignal > BUMP_DET){
		return LIGHTB_R;
	}
	else if(sdata->lightBumpLeftSignal > BUMP_DET){
		return LIGHTB_L;
	}

	return 0;
}

///Tests for physical contact with the bot's bumpers
/**
 * 		Tests for physical contact with the bot's bumpers
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
static int test_bump(){
	if(sdata->bumpLeft){
		return BUMP_L;
	}
	else if(sdata->bumpRight){
		return BUMP_R;
	}

	return 0;
}

/// Tests all the sensors on the bot for move_forward()
/**
 * 		Used during the move_forward() function to return non-zero if a hazard is sensed.
 * 		Will send a string of the first test case that is detected over UART.
 * 		@param[in]  override Will override the testing of an individual sensor.
 * 		@param[out] hazard	Returns non-zero on all hazard detections if override is not set.
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
int test_sensors(uint8_t override){
	int cliff = test_cliff();
	int lightbump = test_lightbump();
	int line = test_line();
	int bump = test_bump();

	if (cliff) {
		switch(cliff){
			case CLIFF_FL:
				sprintf(buf, "i : FL CLIFF!!!: %d\r\n",sdata->cliffFrontLeft);
				uart_puts(buf);
				break;
			case CLIFF_FR:
				sprintf(buf, "i : FR CLIFF!!!: %d\r\n",sdata->cliffFrontRight);
				uart_puts(buf);
				break;
			case CLIFF_R:
				sprintf(buf, "i : R CLIFF!!!: %d\r\n",sdata->cliffRight);
				uart_puts(buf);
				break;
			case CLIFF_L:
				sprintf(buf, "i : L CLIFF!!!: %d\r\n",sdata->cliffLeft);
				uart_puts(buf);
				break;
			default: //Doesn't run
				break;
			}

	} else if (lightbump) {
		switch(lightbump){
		case LIGHTB_CL:
			sprintf(buf, "i : CL-lbump: %d\r\n",sdata->lightBumpCenterLeftSignal);
			uart_puts(buf);
			sprintf(buf, "G 3\n");
			uart_puts(buf);
			break;
		case LIGHTB_FL:
			sprintf(buf, "i : FL-lbump: %d\r\n",sdata->lightBumpFrontLeftSignal);
			uart_puts(buf);
			sprintf(buf, "G 2\n");
			uart_puts(buf);
			break;
		case LIGHTB_CR:
			sprintf(buf, "i : CR-lbump: %d\r\n",sdata->lightBumpCenterRightSignal);
			uart_puts(buf);
			sprintf(buf, "G 4\n");
			uart_puts(buf);
			break;
		case LIGHTB_FR:
			sprintf(buf, "i : FR-lbump: %d\r\n",sdata->lightBumpFrontRightSignal);
			uart_puts(buf);
			sprintf(buf, "G 5\n");
			uart_puts(buf);
			break;
		case LIGHTB_R:
			sprintf(buf, "i : R-lbump: %d\r\n",sdata->lightBumpLeftSignal);
			uart_puts(buf);
			sprintf(buf, "G 6\n");
			uart_puts(buf);
			break;
		case LIGHTB_L:
			sprintf(buf, "i : L-lbump: %d\r\n",sdata->lightBumpLeftSignal);
			uart_puts(buf);
			sprintf(buf, "G 1\n");
			uart_puts(buf);
			break;
		default: //Doesn't run
			break;
		}

	} else if (line) {
		switch(line){
			case LINE_FL:
				sprintf(buf, "i : FL-line: %d\r\n",sdata->cliffFrontLeftSignal);
				uart_puts(buf);
				sprintf(buf, "N 2\n");
				uart_puts(buf);
				break;
			case LINE_FR:
				sprintf(buf, "i : FR-line: %d\r\n",sdata->cliffFrontRightSignal);
				uart_puts(buf);
				sprintf(buf, "N 3\n");
				uart_puts(buf);
				break;
			case LINE_R:
				sprintf(buf, "i : R-line: %d\r\n", sdata->cliffRightSignal);
				uart_puts(buf);
				sprintf(buf, "N 4\n");
				uart_puts(buf);
				break;
			case LINE_L:
				sprintf(buf, "i : L-line: %d\r\n",sdata->cliffLeftSignal);
				uart_puts(buf);
				sprintf(buf, "N 1\n");
				uart_puts(buf);
				break;
			default: //Doesn't run
				break;
		}
	} else if (bump) {
		if(bump == BUMP_L){
			sprintf(buf, "i : Left Bumper: %d\r\n", sdata->bumpLeft);
			uart_puts(buf);
		}
		else{
			sprintf(buf, "i : Right Bumper: %d\r\n", sdata->bumpRight);
			uart_puts(buf);
		}
	}

	return cliff || ((override != 1 || override != 4) && lightbump) || ((override != 4) && line) || ((override != 2 || override != 3) && bump);
}

///Rotates the bot count-clockwise by a given degrees.
/**
 * 		Rotates the bot count-clockwise by a given degrees.
 * 		@param[in]  deg		The degrees desired to rotate.
 *		@param[out] rotate 	The total degrees rotated.
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
uint16_t rotate_cclock(uint16_t deg){
	oi_setWheels(150,-150);
	int sum = 0;

	while(sum < deg) {
		oi_update(sdata);
		sum += abs(sdata->angle);

		if(test_cliff() && test_bump()){
			sprintf(buf, "Bad rotation: %d\r\n", sum);
			uart_puts(buf);

			oi_setWheels(0, 0);
			break;
		}
	}

	oi_setWheels(0, 0); // stop

	return sum;
}

///Rotates the bot clockwise by a given degrees.
/**
 * 		Rotates the bot clockwise by a given degrees.
 * 		@param[in]  deg		The degrees desired to rotate.
 * 		@param[out] rotate 	The total degrees rotated.
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
uint16_t rotate_clock(uint16_t deg){
	oi_setWheels(-150,150);
	int sum = 0;

	while(sum < deg) {
		oi_update(sdata);
		sum += abs(sdata->angle);

		if(test_cliff() && test_bump()){
			sprintf(buf, "Bad rotation: %d\r\n", sum);
			uart_puts(buf);

			oi_setWheels(0, 0);
			break;
		}

	}

	oi_setWheels(0, 0); // stop

	return sum;
}

///Sends all hazard sensor data over UART.
/**
 * 		Sends all hazard sensor data over UART.
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
void print_sensor_data(){
	oi_update(sdata);

	sprintf(buf, "Left Bumper: %d,Right Bumper: %d\r\n", sdata->bumpLeft, sdata->bumpRight);
	uart_puts(buf);

	sprintf(buf, "L-cliff: %d,FL-cliff: %d,FR-cliff: %d,R-cliff: %d\r\n",sdata->cliffLeft, sdata->cliffFrontLeft,sdata->cliffFrontRight, sdata->cliffRight);
	uart_puts(buf);

	sprintf(buf, "L-line: %d,FL-line: %d,FR-line: %d,R-line: %d\r\n",sdata->cliffLeftSignal, sdata->cliffFrontLeftSignal,sdata->cliffFrontRightSignal, sdata->cliffRightSignal);
	uart_puts(buf);

	sprintf(buf, "L-lbump: %d,FL-lbump: %d,CL-lbump: %d\r\n",sdata->lightBumpLeftSignal, sdata->lightBumpFrontLeftSignal,sdata->lightBumpCenterLeftSignal);
	uart_puts(buf);

	sprintf(buf, "R-lbump: %d,FR-lbump: %d,CR-lbump: %d\r\n",sdata->lightBumpRightSignal, sdata->lightBumpFrontRightSignal,sdata->lightBumpCenterRightSignal);
	uart_puts(buf);
}

///Rotates the bot counter-clockwise, at a faster speed, a given degrees.
/**
 * 		Rotates the bot counter-clockwise, at a faster speed, a given degrees.
 * 		@param[in]  deg		Given degrees desired to rotate.
 * 		@param[out] rotate	The total degrees rotated.
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
uint16_t fast_rotate_cclock(uint16_t deg){
	oi_setWheels(350,-350);
	int sum = 0;

	while(sum < deg) {
		oi_update(sdata);
		sum += abs(sdata->angle);

		if(test_cliff() && test_bump()){
			sprintf(buf, "Bad rotation: %d\r\n", sum);
			uart_puts(buf);

			oi_setWheels(0, 0);
			break;
		}
	}

	oi_setWheels(0, 0); // stop

	return sum;
}

///Rotates the bot counter-clockwise, at a faster speed, a given degrees.
/**
 * 		Rotates the bot counter-clockwise, at a faster speed, a given degrees.
 * 		@param[in]  deg		Given degrees desired to rotate.
 * 		@param[out] rotate	The total degrees rotated.
 *
 *		@author Team Drop Tables
 *		@date 4/26/2017
 */
uint16_t fast_rotate_clock(uint16_t deg){
	oi_setWheels(-350,350);
	int sum = 0;

	while(sum < deg) {
		oi_update(sdata);
		sum += abs(sdata->angle);

		if(test_cliff() && test_bump()){
			sprintf(buf, "Bad rotation: %d\r\n", sum);
			uart_puts(buf);

			oi_setWheels(0, 0);
			break;
		}

	}

	oi_setWheels(0, 0); // stop

	return sum;
}
