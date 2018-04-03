#include <asf.h>

#define CONF_UART              UART0
#define CONF_UART_BAUDRATE     9600
#define CONF_UART_CHAR_LENGTH  US_MR_CHRL_8_BIT
#define CONF_UART_PARITY       US_MR_PAR_NO
#define CONF_UART_STOP_BITS    US_MR_NBSTOP_1_BIT
#define	PINO_LED_AZUL	PIO_PA19
#define PINO_LED_VERDE	PIO_PA20

void inicializacao_UART (){
	
	static usart_serial_options_t usart_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits = CONF_UART_STOP_BITS
	};
	usart_serial_init(CONF_UART, &usart_options);
	stdio_serial_init((Usart *)CONF_UART, &usart_options);
}

void menu()
{
	printf("\n\r");
	puts("a: Acender LED azul\r");
	puts("s: Apagar LED azul\r");
	puts("v: Acender LED verde\r");
	puts("b: Apagar LED verde\r");
}

int main (void)
{
	sysclk_init();
	board_init();
	inicializacao_UART();
		
	pio_set_output(PIOA, PINO_LED_AZUL, HIGH, DISABLE, ENABLE);	
	pio_set_output(PIOA, PINO_LED_VERDE, HIGH, DISABLE, ENABLE);

	menu();

	while(1)
	{
		
		char key = getchar();
		switch (key){
			case 'a':
				pio_clear(PIOA, PINO_LED_AZUL);
				puts("LED azul agora esta ACESO\r");
				break;
			case 'v':
				pio_clear(PIOA, PINO_LED_VERDE);
				puts("LED verde agora esta ACESO\r");
				break;
			case 's':
				pio_set(PIOA, PINO_LED_AZUL);
				puts("LED azul agora esta APAGADO\r");
				break;
			case 'b':
				pio_set(PIOA, PINO_LED_VERDE);
				puts("LED verde agora esta APAGADO\r");
				break;
			case 'm':
				menu();
				break;
		}
	}
}
