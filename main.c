/**
 *		@file main.c
 *		@brief this controls the main code for the iRobot control
 *
 *		@author Team Drop Tables
 *
 *		@data 4/26/2017
 */

#include "timer.h"
#include "lcd.h"
#include "button.h"
#include "serial_wifi.h"
#include "math.h"
#include "ir.h"
#include "ping.h"
#include "servo.h"
#include "scan.h"
#include "open_interface.h"
#include "movement.h"
#include "music.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"

/// The main() controls communcation with the provided C# program interface for bot control.
/**
 *		The main() controls communcation with the provided C# program interface for bot control. The command lists are as follows:
 *
 *		S: Scan
 *		m: Move forward 50mm
 *		M: Move forward 300mm
 *		l: Rotate left 10 degrees
 *		L: Rotate left 90 degrees
 *		r: Rotate right 10 degrees
 *		R: Rotate right 90 degrees
 *		D: Send sensor data (Bumper, Cliff, Line, Light Bumper)
 *		X: Play Song 1 (Melee Theme)
 *		Y: Play Song 2 (Gamecube Boot Theme)
 *		V: Victory Dance
 */
int main(void) {
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);

	lcd_init();
	button_init();
	uart_init();
	wifi_start();
	ir_init();
	servo_init();
	ping_init();
	set_sensor_data();


	load_songs();


	oi_play_song(GAMECUBE_THEME);

	char rx;

	for(;;){
		lcd_printf("waiting...\n");
		rx = uart_receive();

		char buf[80];

		switch(rx){
			case 'S' :
				lcd_printf("Scanning...\n");
				scan_action();
				sprintf(buf, " Scan Complete\n");
				uart_puts(buf);
				break;
			case 'm' :
				move_forward(50,0);
				lcd_printf("Moving forward...\n");
				sprintf(buf, "M 50 Moved forward 50mm\n");
				uart_puts(buf);
				break;
			case 'M':
				move_forward(300,0);
				lcd_printf("Moving forward...\n");
				sprintf(buf, "M 300 Moved forward 300mm\n");
				uart_puts(buf);
				break;
			case 'l':
				rotate_cclock(10);
				lcd_printf("Rotating Left...\n");
				sprintf(buf, "L 10 Rotated counter-clockwise 10 degrees\n");
				uart_puts(buf);
				break;
			case 'r':
				rotate_clock(10);
				lcd_printf("Rotating Right...\n");
				sprintf(buf, "R 10 Rotated clockwise 10 degrees\n");
				uart_puts(buf);
				break;
			case 'L':
				rotate_cclock(90);
				lcd_printf("Rotating Left...\n");
				sprintf(buf, "L 90 Rotated counter-clockwise 90 degrees\n");
				uart_puts(buf);
				break;
			case 'R':
				rotate_clock(90);
				lcd_printf("Rotating Right...\n");
				sprintf(buf, "R 90 Rotated clockwise 90 degrees\n");
				uart_puts(buf);
				break;
			case 'D':
				lcd_printf("Sending Sensor Data...\n");
				print_sensor_data();
				break;
			case 'X':
				oi_play_song(MELEE_THEME);
				lcd_printf("Fox only, \nfinal destination\n");
				sprintf(buf, "Playing song\n");
				uart_puts(buf);
				break;
			case 'Y':
				oi_play_song(GAMECUBE_THEME);
				lcd_printf("Loading OS...\n");
				sprintf(buf, "Playing song\n");
				uart_puts(buf);
				break;
			case 'V':
				oi_play_song(MELEE_THEME);
				fast_rotate_clock(360);
				fast_rotate_cclock(360);
				move_forward(50,0);
				move_backward(100);
				move_forward(50,0);
				fast_rotate_clock(360);
				fast_rotate_cclock(360);
		}

		timer_waitMillis(500);
	}

}
