#ifndef SCAN_H_
#define SCAN_H_

#define PI 3.14159265


#include <stdint.h>

#include <inc/tm4c123gh6pm.h>

// Resets all the variables for consistent scan readings
void scan_reset();

// Scans 180 degree arc and marks detected objects
void scan_action();


#endif /* SCAN_H_ */
