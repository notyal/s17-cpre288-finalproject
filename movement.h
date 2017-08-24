/**
 *		@file movement.h
 *		@brief this header file will contain all the required
 *		definitions and basic movement functions
 *
 *		@author Team Drop Tables
 *
 *		@date April 26, 2017
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "open_interface.h"
#include "timer.h"
#include <stdint.h>

/**
 * Test constants for line and bumper detection
 */
#define LINE_DET 2800
#define BUMP_DET 700

/**
 * Return constants to differentiate between what hazard sensors have triggered
 */
///@{
#define CLIFF_FL 1
#define CLIFF_FR 2
#define CLIFF_L 3
#define CLIFF_R 4

#define LIGHTB_CL 10
#define LIGHTB_FL 11
#define LIGHTB_FR 12
#define LIGHTB_CR 13
#define LIGHTB_R 14
#define LIGHTB_L 15

#define LINE_FL 20
#define LINE_FR 21
#define LINE_L 22
#define LINE_R 23

#define BUMP_L 31
#define BUMP_R 32
///@}

/**
 * Global variables to be used across all functions
 * sdata is the all the bot's open interface data structure.
 * buf is the character string sent over UART
 */
oi_t *sdata;
char buf[128];

void set_sensor_data(void);
void free_sensor_data(void);
uint16_t move_forward(uint16_t mm, uint8_t override);
uint8_t move_backward(uint8_t mm);

uint16_t rotate_clock(uint16_t deg);
uint16_t rotate_cclock(uint16_t deg);
int test_sensors(uint8_t override);
int test_lightbump(void);
void print_sensor_data();
uint16_t fast_rotate_cclock(uint16_t deg);
uint16_t fast_rotate_clock(uint16_t deg);



#endif
