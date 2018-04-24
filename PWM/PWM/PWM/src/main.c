#include <asf.h>

#define PWM_DC 50
#define PWM_PERIOD 100
#define PWM_FREQ 1000
#define CANAL_PWM PIN_PWM_LED0_CHANNEL
pwm_channel_t g_pwm_channel_led;

void pwm_setup(void)
{

	pmc_enable_periph_clk(ID_PWM);
	pwm_channel_disable(PWM, CANAL_PWM);
	pwm_clock_t clock_setting = {
		.ul_clka = PWM_PERIOD * PWM_FREQ,
		.ul_clkb = 0,
		.ul_mck = sysclk_get_cpu_hz()
	};
	pwm_init(PWM, &clock_setting);
	NVIC_DisableIRQ(PWM_IRQn);
	NVIC_ClearPendingIRQ(PWM_IRQn);
	NVIC_SetPriority(PWM_IRQn, 0);
	NVIC_EnableIRQ(PWM_IRQn);
	pwm_channel_enable_interrupt(PWM, CANAL_PWM, 0);
	g_pwm_channel_led.channel = CANAL_PWM;
	g_pwm_channel_led.ul_prescaler = PWM_CMR_CPRE_CLKA;
	g_pwm_channel_led.ul_period = PWM_PERIOD;
	g_pwm_channel_led.ul_duty = PWM_DC;	
	g_pwm_channel_led.polarity = PWM_LOW;
	g_pwm_channel_led.alignment = PWM_ALIGN_LEFT;
	pwm_channel_init(PWM, &g_pwm_channel_led);
	pwm_channel_enable(PWM, CANAL_PWM);

}

void PWM_Handler(void)
{
	static uint32_t ul_count = 0;  /* PWM counter value */
	static uint32_t ul_duty = PWM_DC;  /* PWM duty cycle rate */
	static uint8_t fade_in = 1;  /* LED fade in flag */

	uint32_t events = pwm_channel_get_interrupt_status(PWM);

	/* Interrupt on PIN_PWM_LED0_CHANNEL */
	if ((events & (1 << PIN_PWM_LED0_CHANNEL)) ==
	(1 << PIN_PWM_LED0_CHANNEL)) {
		ul_count++;

		/* Fade in/out */
		if (ul_count == (PWM_FREQ / (PWM_PERIOD - PWM_DC))) {
			/* Fade in */
			if (fade_in) {
				ul_duty++;
				if (ul_duty == PWM_PERIOD) {
					fade_in = 0;
				}
				} else {
				/* Fade out */
				ul_duty--;
				if (ul_duty == PWM_DC) {
					fade_in = 1;
				}
			}

			/* Set new duty cycle */
			ul_count = 0;
			g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
			pwm_channel_update_duty(PWM, &g_pwm_channel_led, ul_duty);
			g_pwm_channel_led.channel = PIN_PWM_LED1_CHANNEL;
			pwm_channel_update_duty(PWM, &g_pwm_channel_led, ul_duty);
		}
	}
}


int main (void)
{
	sysclk_init();
	board_init();
	pwm_setup();
	while(1)
	{

	}
}
