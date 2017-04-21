#include "scan.h"

#include "math.h"
#include "ir.h"
#include "ping.h"
#include "servo.h"
#include "timer.h"
#include "button.h"
#include "serial_wifi.h"
#include <stdbool.h>


int16_t scan_degrees = 0;
int8_t obj_count = 0;
_Bool obj_detection = false;
int16_t deg_start = 0;
int16_t deg_end = 0;
int16_t temp_deg_end = 0;
int16_t deg_width = 0;
float distance_total = 0;
float distance_avg = 0;
int16_t distance_count = 0;
float object_width = 0;
int16_t deg_smallest_obj = 0;
float smallest_obj_width = 100;
int8_t edge_count = 0;
_Bool end_case = false;


void scan_reset(){
	scan_degrees = 0;
	obj_detection = false;
	deg_start = 0;
	deg_end = 0;
	temp_deg_end = 0;
	deg_width = 0;
	distance_total = 0;
	distance_avg = 0;
	distance_count = 0;
	object_width = 0;
	deg_smallest_obj = 0;
	smallest_obj_width = 100;
	edge_count = 0;
	end_case = false;

	// Sets servo back to reset zero, waits to prevent false readings
	move_servo(0);
	timer_waitMillis(250);
}

void scan_action(){
	scan_reset();

	while(scan_degrees < 180){
		float ir_dist = ir_getDistance();
		float ping_dist = ping_read();


		// Used to prevent false positives in detecting edge ends
		if((ir_dist > 80 || ping_dist > 90) && obj_detection && !end_case) {
			temp_deg_end = scan_degrees;
			edge_count++;
			end_case = true;
		} else if((ir_dist > 80 || ping_dist > 90) && obj_detection && end_case) {
			edge_count++;
		}

		// calls when a new object is spotted
		if ( (ir_dist <= 80) && ping_dist <= 90 && !obj_detection) {
			obj_detection = true;
			deg_start = scan_degrees;
			deg_end = 0;
			obj_count++;
			distance_avg = 0;
		}

		// calls when an object is lost in sight
		else if ( (ir_dist > 80 || ping_dist > 90) && obj_detection && edge_count > 1) {
			obj_detection = false;
			deg_end = temp_deg_end;
			deg_width = abs(deg_end - deg_start); // calculate object width in degrees

			object_width = ( distance_avg * sin((deg_width) * (PI/180) ) ) / ( sin(( (180-deg_width)/2) * (PI/180) ) );

			if(object_width < smallest_obj_width){
				smallest_obj_width = object_width;
				deg_smallest_obj = deg_start;
			}

			// Transmit data over UART
			int dist_send = (int) distance_avg;

			char buf[80];
			sprintf(buf, "S %d %d %d\r\n", deg_start, deg_end,dist_send);
			uart_puts(buf);

			// Reset error protection
			distance_count = distance_total = 0;
			edge_count = 0;
			end_case = false;
		}

		// Calculates running average of the distance from sensor to object
		if(obj_detection){
			distance_count++;
			distance_total += ping_dist;
			distance_avg = distance_total / distance_count;
		}

		// Increments servo by a degree
		scan_degrees = scan_degrees + 1;
		move_servo(scan_degrees);
		timer_waitMillis(100);
	}

	move_servo(90);
}
