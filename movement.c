#include "movement.h"
#include "open_interface.h"
#include "timer.h"
#include <stdlib.h>
#include <stdint.h>


/**
 * Allocates storage for open interface data
 */
void set_sensor_data(void){
	sdata = oi_alloc();
	oi_init(sdata);
}
/**
 * Frees open interface data
 */
void free_sensor_data(void){
	oi_free(sdata);
}

/**
 * Move back 30 cm
 */
static void back_away(){
	oi_setWheels(-50, -50);

	timer_waitMillis(3000);

	oi_setWheels(0,0);
}
/**
 * Moves forward given millimeters while checking sensors.
 */
uint8_t move_forward(uint8_t mm, uint8_t override){
	uint8_t sum=0;

	oi_setWheels(100, 100);

	while(sum < mm){
		oi_update(sdata);

		sum += sdata->distance;

		if(test_sensors(override)){
//			back_away();
			move_backward(30);
			break;
		}
	}

	oi_setWheels(0,0);

	return sum;
}

/**
 * Moves backward given millimeters
 */
uint8_t move_backward(uint8_t mm){
	int sum=0;

	oi_setWheels(-50, -50);

	while(sum < mm){
		oi_update(sdata);

		sum += abs(sdata->distance);

//		if(test_sensors()){
//			back_away();
//			break;
//		}
	}

	oi_setWheels(0,0);

	return sum;
}
/**
 * Tests the cliff sensors
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
/**
 * Tests for line detection
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
/**
 * Tests light sensor in bumpers for distance
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
/**
 * Tests for physical contact with the bot's bumpers
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

/**
 * Tests all senors for hazards,
 * 	Unless the override bit is set
 */
int test_sensors(uint8_t override){
//	return test_cliff() || ((override != 1) && test_lightbump()) || test_line() || ((override != 2 || override != 3) && test_bump());
	return test_cliff() || test_lightbump() || test_line() || test_bump();
}

/**
 * Rotates bot counter-clockwise
 */
uint16_t rotate_cclock(uint16_t deg){
	oi_setWheels(150,-150);
	int sum = 0;

	while(sum < deg) {
		oi_update(sdata);
		sum += abs(sdata->angle);
	}

    oi_setWheels(0, 0); // stop

    return sum;
}

/**
 * Rotates bot clockwise
 */
uint16_t rotate_clock(uint16_t deg){
	oi_setWheels(-150,150);
	int sum = 0;

	while(sum < deg) {
		oi_update(sdata);
		sum += abs(sdata->angle);
	}

    oi_setWheels(0, 0); // stop

    return sum;
}
