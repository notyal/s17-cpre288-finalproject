#ifndef MAIN_H_
#define MAIN_H_

#define PI 3.14159265

void ADC0SS0_Handler(void);
void adc_init(void);
unsigned adc_read(char);
double sensor_correct(unsigned int);

unsigned imove_servo(int);

#endif /* MAIN_H_ */
