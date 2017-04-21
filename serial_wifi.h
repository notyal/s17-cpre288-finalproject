#ifndef SERIAL_WIFI_H_
#define SERIAL_WIFI_H_

void uart_init(void);
void uart_sendChar(char);
char uart_receive(void);
void uart_puts(char data[]);
void wifi_start();
void wifi_stop();

#endif /* SERIAL_WIFI_H_ */
