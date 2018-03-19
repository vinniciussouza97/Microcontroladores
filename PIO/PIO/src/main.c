#include <asf.h>
#define LED_VERDE (1<<20)
#define LED_AZUL (1<<19)

int main (void)
{
	sysclk_init();
	board_init();
	
	PIOA->PIO_PER |= LED_VERDE|LED_AZUL;
	PIOA->PIO_OER |= LED_VERDE|LED_AZUL;
	PIOA->PIO_SODR |= LED_VERDE;
	PIOA->PIO_CODR |= LED_AZUL;

	while(1)
	{
		
	}
}
