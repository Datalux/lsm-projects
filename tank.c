#include "stm32_unict_lib.h"

#define MAX_LEVEL 500
#define ON 		1
#define OFF 	0


int water_level;
int value;
int timer;
int valvola;


void main(void)
{
	DISPLAY_init();
	TIM_init(TIM2);
	ADC_init(ADC1, ADC_RES_8, ADC_ALIGN_RIGHT);
	GPIO_init(GPIOB);

	TIM_config_timebase(TIM2, 8400, 100); // timer al centesimo di secondo
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_set(TIM2, 0);
	TIM_on(TIM2);
	GPIO_config_output(GPIOB, 0);
	GPIO_config_input(GPIOB, 10); // X
	GPIO_config_input(GPIOB, 4);  // Y

	ADC_channel_config(ADC1, GPIOC, 1, 11);
	ADC_channel_config(ADC1, GPIOC, 0, 10);
	ADC_on(ADC1);

	GPIO_config_EXTI(GPIOB, EXTI10);
	EXTI_enable(EXTI10, FALLING_EDGE);
	GPIO_config_EXTI(GPIOB, EXTI4);
	EXTI_enable(EXTI4, FALLING_EDGE);



	ADC_sample_channel(ADC1, 10);

	water_level = 0;
	value = 0;
	timer = 0;
	valvola = ON;

	for (;;) {
		char s[5];
		sprintf(s, "%4d", water_level);
		DISPLAY_puts(0,s);
		switch(valvola){
		case ON:
			GPIO_write(GPIOB, 0, 0);
			break;
		case OFF:
			GPIO_write(GPIOB, 0, 1);
			break;
		}
	}
}


void TIM2_IRQHandler(void){
	if(TIM_update_check(TIM2)){
		timer++;
		if(timer == 100){
			ADC_sample_channel(ADC1, 10);
			ADC_start(ADC1);
			while(!ADC_completed(ADC1)) {}
			value = (50 * ADC_read(ADC1))/255;
			if(valvola == ON){
				if(water_level >= MAX_LEVEL - value)
					water_level = MAX_LEVEL;
				else
					water_level += value;
			}

			ADC_sample_channel(ADC1, 11);
			ADC_start(ADC1);
			while(!ADC_completed(ADC1)) {}
			value = (10 * ADC_read(ADC1))/255;
			if(water_level <= value)
				water_level = 0;
			else
				water_level -= value;

			timer = 0;
			TIM_set(TIM2, 0);
		}
		if(water_level > 430)	valvola = OFF;
		if(water_level < 150)	valvola = ON;
		TIM_update_clear(TIM2);
	}
}

void EXTI15_10_IRQHandler(void)	/* tasto X */
{
	if (EXTI_isset(EXTI10)) {
		if(water_level != MAX_LEVEL)
			valvola = ON;
		EXTI_clear(EXTI10);
	}
}


void EXTI4_IRQHandler(void) /* tasto Y */
{
	if(EXTI_isset(EXTI4)){
		valvola = OFF;
		EXTI_clear(EXTI4);
	}
}


