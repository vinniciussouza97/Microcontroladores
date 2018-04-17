#include <asf.h>

#define TC			TC0
#define CHANNEL		0
#define ID_TC		ID_TC0
#define TC_Handler  TC0_Handler
#define TC_IRQn     TC0_IRQn

#define CONF_UART              UART0
#define CONF_UART_BAUDRATE     9600
#define CONF_UART_CHAR_LENGTH  US_MR_CHRL_8_BIT
#define CONF_UART_PARITY       US_MR_PAR_NO
#define CONF_UART_STOP_BITS    US_MR_NBSTOP_1_BIT

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

/**
 *  Configure Timer Counter 0 to generate an interrupt every 250ms.
 */
static void tc_config(uint32_t freq_desejada)
{
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t counts;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();
	
	pmc_enable_periph_clk(ID_TC0);
	
	tc_find_mck_divisor( freq_desejada, ul_sysclk, &ul_div, &ul_tcclks,	ul_sysclk);
	
	tc_init(TC0, 0, ul_tcclks | TC_CMR_CPCTRG);

	unsigned int clock_timer = sysclk_get_peripheral_bus_hz(TC0);
	printf("Clock do periferico SCLK: %u \n\r", clock_timer);
	
	counts = (ul_sysclk/ul_div)/freq_desejada;
	
	printf("counts: %u \n\r", counts);
	tc_write_rc(TC0, 0, counts);

	NVIC_DisableIRQ(TC0_IRQn);
	NVIC_ClearPendingIRQ(TC0_IRQn);
	NVIC_SetPriority(TC0_IRQn,2);
	NVIC_EnableIRQ(TC0_IRQn);
	
	// Enable interrupts for this TC, and start the TC.
	tc_enable_interrupt(TC,	CHANNEL,
	TC_IER_CPCS);				// Enable interrupt.
	tc_start(TC,CHANNEL);			// Start the TC.


}


// Interrupt service routine
void TC0_Handler(void)
{
	tc_get_status(TC0,0);
	puts("Execução da ISR do timer.");
	LED_Toggle(LED0_GPIO);
}


int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();
	board_init();

	inicializacao_UART();
	tc_config(1);
	
	//Configura os PIOs de saida (LEDs)
	pio_set_output(PIOA, PIO_PA19, LOW, DISABLE, ENABLE);
	pio_set_output(PIOA, PIO_PA20, LOW, DISABLE, ENABLE);
	
	while(1) {
		
	}
	
}
