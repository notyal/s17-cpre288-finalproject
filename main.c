#include "main.h"
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
#include "sensor.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"
//#include <unistd.h>

uint8_t idegrees = 0;
unsigned imove_servo(int deg){
	idegrees = deg;
	return move_servo(deg);
}



  int main(void) {
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
//	lcd_init();
//
//	while(1){
//		lcd_printf("%d\n%d\n%d\n%d", sensor_data->cliffLeft,sensor_data->cliffFrontLeft,sensor_data->cliffFrontRight,sensor_data->cliffRight);
//		oi_update(sensor_data);
//	}

	lcd_init();
	button_init();
	uart_init();
	wifi_start();
	ir_init();
	servo_init();
	ping_init();
	set_sensor_data();

	// wait for button 6 to start
	lcd_clear();
	lcd_printf("\nPress Button 6\nTo Start\n");
	while (button_getButton() != 6);
	lcd_clear();

	char rx;

	for(;;){
		lcd_printf("waiting...");
		rx = uart_receive();

		char buf[80];

		switch(rx){
			case 'S' :
				scan_action();
				break;
			case 'M':
				move_forward(20,0);

				sprintf(buf, "M Moved forward 20cm");
				uart_puts(buf);
				break;
			case 'L':
				rotate_cclock(10);

				sprintf(buf, "L Rotated counter-clockwise 5 degrees");
				uart_puts(buf);
				break;
			case 'R':
				rotate_clock(10);

				sprintf(buf, "R Rotated clockwise 5 degrees");
				uart_puts(buf);
				break;
		}

		timer_waitMillis(500);
	}

}



//	for(;;){
////		uart_sendChar(command[0]);
//		int override = 0;
//		uint8_t dist = 0;
//
//		switch(uart_receive()){
//			case 'S' :
//				scan_action();
//				break;
//			case 'M':
//				override = atoi((const char)uart_receive());
//				char r;
//				char rdata[255];
//				uint8_t i = 0;
//				for(i = 0, r = uart_receive(); r != '\n'; i++) {
//					rdata[0] = r;
//				}
//				rdata[i] = '\0';
//				dist = atoi(rdata);
//				break;
//		}
//	}
