#include <asf.h>

#define LED_VERDE (1<<20)
#define LED_AZUL (1<<19)
#define BOTAO (1<<3)

int main (void)
{
	sysclk_init();
	board_init();
	
	PIOA->PIO_PER |= LED_VERDE|LED_AZUL;
	PIOB->PIO_PER |= BOTAO;
	PIOA->PIO_OER |= LED_VERDE|LED_AZUL;
	PIOB->PIO_ODR |= BOTAO;
	PIOA->PIO_SODR |= LED_VERDE|LED_AZUL;

	while(1)
	{		
//		delay_ms(250);
//		PIOA->PIO_SODR |= LED_VERDE|LED_AZUL;
//		delay_ms(250);
//		PIOA->PIO_CODR |= LED_VERDE|LED_AZUL;
		if (PIOB->PIO_PDSR & BOTAO) //"&" limpa todas as casas exceto a selecionada
		{
			PIOA->PIO_SODR |= LED_VERDE|LED_AZUL;
		}
		else
		{
			PIOA->PIO_CODR |= LED_VERDE|LED_AZUL;
		}

	}
}
