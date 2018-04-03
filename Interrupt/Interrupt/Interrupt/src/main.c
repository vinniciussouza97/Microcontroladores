#include <asf.h>

#define PINO_LED_AZUL PIO_PA19

void tratamento_interrupcao_pioB();
void configurar_botao1 ();

void configurar_botao1 (){
	pio_set_input(PIOB, PIO_PB3, PIO_PULLUP | PIO_DEBOUNCE);
	pio_handler_set(PIOB, ID_PIOB, PIO_PB3, PIO_IT_RISE_EDGE, tratamento_interrupcao_pioB);
	pio_enable_interrupt(PIOB, PIO_PB3);

	NVIC_SetPriority (PIOB_IRQn,15);
	NVIC_EnableIRQ(PIOB_IRQn);
}


int main (void)
{
	sysclk_init();
	board_init();
	configurar_botao1();
	pio_set_output(PIOA,PINO_LED_AZUL,LOW,DISABLE,ENABLE);

	while(1){
		
	}

}

void tratamento_interrupcao_pioB(const uint32_t id, const uint32_t index){

	if(pio_get(PIOA,PIO_TYPE_PIO_OUTPUT_1,PINO_LED_AZUL))
		pio_clear(PIOA,PINO_LED_AZUL);
	else
		pio_set(PIOA,PINO_LED_AZUL);
	
	// = LED_Toggle(LED0_GPIO);

}