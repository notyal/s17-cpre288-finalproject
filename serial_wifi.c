#include "serial_wifi.h"
#include "lcd.h"

//Initialize USART1 to a given baud rate
void uart_init(void) {
	//enable clock to GPIO, R1 = port B
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

	//enable clock to UART1, R1 = UART1. ***Must be done before setting Rx and Tx (See DataSheet)
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;

	//enable alternate functions on port b pins 0 and 1
	GPIO_PORTB_AFSEL_R |= (BIT0 | BIT1);

	//enable Rx and Tx on port B on pins 0 and 1
	GPIO_PORTB_PCTL_R |= 0x00000011;

	//set pin 0 and 1 to digital
	GPIO_PORTB_DEN_R |= (BIT0 | BIT1);

	//set pin 0 to Rx or input
	GPIO_PORTB_DIR_R &= ~BIT0;

	//set pin 1 to Tx or output
	GPIO_PORTB_DIR_R |= BIT1;

	//calculate baudrate
	const uint16_t iBRD = 8;
	const uint16_t fBRD = 44;

	//turn off uart1 while we set it up
	UART1_CTL_R &= ~(UART_CTL_UARTEN);

	//set baud rate
	UART1_IBRD_R = iBRD;
	UART1_FBRD_R = fBRD;

	//set frame, 8 data bits, 1 stop bit, no parity, no FIFO
	UART1_LCRH_R = UART_LCRH_WLEN_8;

	//use system clock as source
	UART1_CC_R = UART_CC_CS_SYSCLK;

	//re-enable enable RX, TX, and uart1
	UART1_CTL_R = (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);

}

//Blocking call that sends 1 char over UART 1
void uart_sendChar(char data) {
	//wait until there is room to send data
	while (UART1_FR_R & 0x20)
		;

	//send data
	UART1_DR_R = data;
}

//Blocking call to receive over uart1
//returns char with data
char uart_receive(void) {
	char data = 0;

	//wait to receive
	while (UART1_FR_R & UART_FR_RXFE)
		;

	//mask the 4 error bits and grab only 8 data bits
	data = (char) (UART1_DR_R & 0xFF);
	return data;
}

//char[] uart_receive_string(void){
//
//return null;
//}

/**
 * Put char array to screen
 */
void uart_puts(char data[]) {
	while (*data != '\0') {
		uart_sendChar(*data);
		data++;
	}
}

void wifi_start() {
	//enable port B on pins 2
	GPIO_PORTB_PCTL_R |= 0x100;

	//set pin 2 to digital
	GPIO_PORTB_DEN_R |= BIT2;

	//set pin 2 output
	GPIO_PORTB_DIR_R |= BIT2;

	// enter command
	GPIO_PORTB_DATA_R |= BIT2;

	uart_sendChar(0x0);
	uart_puts("Z12345678");
	uart_sendChar(0x0);

	char response = uart_receive();

	// exit command
	GPIO_PORTB_DATA_R &= ~BIT2;

	if (response != 0) {
		// an error occured
		lcd_puts("\nWIFI ERROR\n");
	}
}

void wifi_stop() {
	// enter command
	GPIO_PORTB_DATA_R |= BIT2;
	uart_sendChar(0x0);
	char response = uart_receive();
	// exit command
	GPIO_PORTB_DATA_R &= ~BIT2;

	if (response != 0) {
		// an error occured
		lcd_puts("\nWIFI ERROR\n");
	}
}
