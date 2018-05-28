#include <asf.h>

/** Chip select number to be set */
#define ILI93XX_LCD_CS      1

/** ADC **/
#define VOLT_REF        (3300)
#define TRACKING_TIME    15
#define TRANSFER_PERIOD  2
#define STARTUP_TIME ADC_STARTUP_TIME_4
#define MAX_DIGITAL     (4095)
#define ADC_CHANNEL 5
#define ADC_UMIDADE	0

#define TC			TC0
#define CHANNEL		0
#define ID_TC		ID_TC0
#define TC_Handler  TC0_Handler
#define TC_IRQn     TC0_IRQn

struct ili93xx_opt_t g_ili93xx_display_opt;

/************************************************************************/
/* HANDLERS                                                            */
/************************************************************************/
static void push_button_handle(uint32_t id, uint32_t mask)
{
	adc_start(ADC);
}


/**
* \brief ADC interrupt handler.
*/
void ADC_Handler(void)
{
	uint16_t result;
	uint32_t status;

	status = adc_get_status(ADC);

	if (status & ADC_ISR_EOC5)
	{
		result = adc_get_channel_value(ADC,ADC_CHANNEL);

		char buffer[20];
		sprintf (buffer, "Pot: %d", result);

		ili93xx_set_foreground_color(COLOR_WHITE);
		ili93xx_draw_filled_rectangle(95, 175, 240, 200);

		ili93xx_set_foreground_color(COLOR_BLACK);
		ili93xx_draw_string(100, 180, (uint8_t*) buffer);

	}
	else if (status & ADC_ISR_EOC0)
	{
		result = adc_get_channel_value(ADC,ADC_UMIDADE);

		char buffer[20];

		sprintf (buffer, "Umi: %d", result);

		ili93xx_set_foreground_color(COLOR_WHITE);
		ili93xx_draw_filled_rectangle(95, 135, 240, 170);

		ili93xx_set_foreground_color(COLOR_BLACK);
		ili93xx_draw_string(100, 140, (uint8_t*) buffer);
	}
}


void TC_Handler(void)
{
	tc_get_status(TC,CHANNEL);
	adc_start(ADC);
}


/************************************************************************/
/* CONFIGs                                                              */
/************************************************************************/
void configure_lcd()
{
	/** Enable peripheral clock */
	pmc_enable_periph_clk(ID_SMC);

	/** Configure SMC interface for Lcd */
	smc_set_setup_timing(SMC, ILI93XX_LCD_CS, SMC_SETUP_NWE_SETUP(2)
	| SMC_SETUP_NCS_WR_SETUP(2)
	| SMC_SETUP_NRD_SETUP(2)
	| SMC_SETUP_NCS_RD_SETUP(2));
	
	smc_set_pulse_timing(SMC, ILI93XX_LCD_CS, SMC_PULSE_NWE_PULSE(4)
	| SMC_PULSE_NCS_WR_PULSE(4)
	| SMC_PULSE_NRD_PULSE(10)
	| SMC_PULSE_NCS_RD_PULSE(10));
	
	smc_set_cycle_timing(SMC, ILI93XX_LCD_CS, SMC_CYCLE_NWE_CYCLE(10)
	| SMC_CYCLE_NRD_CYCLE(22));
	
	smc_set_mode(SMC, ILI93XX_LCD_CS, SMC_MODE_READ_MODE
	| SMC_MODE_WRITE_MODE);

	/** Initialize display parameter */
	g_ili93xx_display_opt.ul_width = ILI93XX_LCD_WIDTH;
	g_ili93xx_display_opt.ul_height = ILI93XX_LCD_HEIGHT;
	g_ili93xx_display_opt.foreground_color = COLOR_BLACK;
	g_ili93xx_display_opt.background_color = COLOR_WHITE;

	/** Switch off backlight */
	aat31xx_disable_backlight();

	/** Initialize LCD */
	ili93xx_init(&g_ili93xx_display_opt);

	/** Set backlight level */
	aat31xx_set_backlight(AAT31XX_AVG_BACKLIGHT_LEVEL);

	ili93xx_set_foreground_color(COLOR_WHITE);
	ili93xx_draw_filled_rectangle(0, 0, ILI93XX_LCD_WIDTH,
	ILI93XX_LCD_HEIGHT);
	/** Turn on LCD */
	ili93xx_display_on();
	ili93xx_set_cursor_position(0, 0);
}

void configure_botao(void)
{
	pmc_enable_periph_clk(ID_PIOB);
	
	pio_set_input(PIN_PUSHBUTTON_1_PIO, PIN_PUSHBUTTON_1_MASK, PIN_PUSHBUTTON_1_ATTR);
	pio_set_debounce_filter(PIN_PUSHBUTTON_1_PIO, PIN_PUSHBUTTON_1_MASK, 10);
	pio_handler_set(PIN_PUSHBUTTON_1_PIO, PIN_PUSHBUTTON_1_ID,PIN_PUSHBUTTON_1_MASK, PIN_PUSHBUTTON_1_ATTR ,push_button_handle);
	pio_enable_interrupt(PIN_PUSHBUTTON_1_PIO, PIN_PUSHBUTTON_1_MASK);
	NVIC_SetPriority( PIN_PUSHBUTTON_1_ID, 5);
	NVIC_EnableIRQ( PIN_PUSHBUTTON_1_ID);
}


void configure_adc(void)
{
	/* Enable peripheral clock. */
	pmc_enable_periph_clk(ID_ADC);	

	adc_init(ADC, sysclk_get_cpu_hz(), 6400000, STARTUP_TIME);
	adc_configure_timing(ADC, TRACKING_TIME	, ADC_SETTLING_TIME_3, TRANSFER_PERIOD);
	adc_configure_trigger(ADC, ADC_TRIG_SW, 0);
	adc_enable_channel(ADC, ADC_UMIDADE);
	adc_enable_channel(ADC, ADC_CHANNEL);
	NVIC_SetPriority(ADC_IRQn, 5);
	NVIC_EnableIRQ(ADC_IRQn);
	adc_enable_interrupt(ADC, ADC_ISR_EOC5);
	adc_enable_interrupt(ADC, ADC_ISR_EOC0);
}

static void tc_config(uint32_t freq_desejada)
{
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t counts;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();
	
	pmc_enable_periph_clk(ID_TC);
	
	tc_find_mck_divisor( freq_desejada, ul_sysclk, &ul_div, &ul_tcclks,	BOARD_MCK);
	tc_init(TC, CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	counts = (ul_sysclk/ul_div)/freq_desejada;
	tc_write_rc(TC, CHANNEL, counts);
	NVIC_ClearPendingIRQ(TC_IRQn);
	NVIC_SetPriority(TC_IRQn, 4);
	NVIC_EnableIRQ(TC_IRQn);
	tc_enable_interrupt(TC,	CHANNEL, TC_IER_CPCS);
	tc_start(TC, CHANNEL);
}


/************************************************************************/
/* MAIN                                                                 */
/************************************************************************/
int main(void)
{
	sysclk_init();
	board_init();

	configure_lcd();
	configure_adc();
	configure_botao();
	tc_config(10);

	/** Draw text on the LCD */
	ili93xx_set_foreground_color(COLOR_BLACK);
	ili93xx_draw_string(10, 20, (uint8_t *)"Aula - ADC");

	while (1)
	{
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}
