#ifndef PING_H_
#define PING_H_

void ping_init();
unsigned ping_read();
void TIMER3B_Handler(void);
void send_pulse();
unsigned time2dist(unsigned);
void clock_timer_init(void);

#endif /* PING_H_ */
