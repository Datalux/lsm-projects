/*
 * QUESTA È LA PRIMA VERSIONE DEL CODICE,
 * PERTANTO NON È OTTIMIZZATO.
 * IN SEGUITO VERRÀ AGGIORNATO CON LE
 * VARIE MIGLIORIE.
 *
 *	Esercitazione del 22 ottobre 2018
 */

#include "stm32_unict_lib.h"

enum{
	ST_IDLE,
	ST_SETUP,
	ST_RUN,
	ST_STOP
};

#define T_MIN	1
#define T_MAX	20

int status;
int timer;
int counter;
int led_timer;
char s[5];


void main(void)
{
	GPIO_init(GPIOB);
	GPIO_init(GPIOC);
    GPIO_config_input(GPIOB, 10);	/* tasto X */
    GPIO_config_input(GPIOB, 5);	/* tasto Z */
    GPIO_config_input(GPIOB, 6);	/* tasto T */
    GPIO_config_input(GPIOB, 4);	/* tasto Y */
    GPIO_config_output(GPIOB, 8);	/* due punti sul display */
    GPIO_config_output(GPIOB, 0);
    GPIO_config_output(GPIOC, 3);

    GPIO_config_EXTI(GPIOB, EXTI10);
    EXTI_enable(EXTI10, FALLING_EDGE);

    GPIO_config_EXTI(GPIOB, EXTI5);
    EXTI_enable(EXTI5, FALLING_EDGE);

    GPIO_config_EXTI(GPIOB, EXTI6);
    EXTI_enable(EXTI6, FALLING_EDGE);

    GPIO_config_EXTI(GPIOB, EXTI4);
    EXTI_enable(EXTI4, FALLING_EDGE);

	TIM_init(TIM2);
	DISPLAY_init();

	TIM_config_timebase(TIM2, 8400, 100);

	TIM_on(TIM2);
	TIM_set(TIM2, 0);

    TIM_enable_irq(TIM2, IRQ_UPDATE);

	status = ST_IDLE;
	timer = T_MIN;
	counter = 0;
	led_timer = 0;

	while(1) {
		switch(status){
		case ST_IDLE:
			sprintf(s,"%2d00", timer);
			DISPLAY_puts(0, s);
			GPIO_write(GPIOB, 8, 1);
			GPIO_write(GPIOB, 0, 0);
			break;
		case ST_SETUP:
			sprintf(s,"%2d00", timer);
			DISPLAY_puts(0, s);
			GPIO_write(GPIOB, 8, 1);
			GPIO_write(GPIOB, 0, 0);
			break;
		case ST_RUN:
			if(counter > (timer*100)-100){
				sprintf(s," 0%2d", (timer*100)-counter);
				DISPLAY_puts(0, s);
				GPIO_write(GPIOB, 8, 1);
			}
			else{
				sprintf(s,"%4d", (timer*100)-counter);
				DISPLAY_puts(0, s);
				GPIO_write(GPIOB, 8, 1);
			}
			break;
		}
	}
}

void TIM2_IRQHandler(void){
	if(TIM_update_check(TIM2)){
		if(status == ST_RUN){
			//GPIO_write(GPIOB, 0, 0);
			++counter;
			++led_timer;
			if(led_timer == 50){
				GPIO_toggle(GPIOB, 0);
				led_timer = 0;
			}
			if(counter/100 >= timer){
				status = ST_IDLE;
				TIM_set(TIM2, 0);
				GPIO_write(GPIOB, 0, 0);
				GPIO_write(GPIOC, 3, 1);
				counter = 0;
			}

		}
		TIM_update_clear(TIM2);
	}
}

// tasto 'X'
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_isset(EXTI10)) {
		switch(status){
		case ST_IDLE:
			status = ST_SETUP;
			break;
		case ST_SETUP:
			status = ST_IDLE;
			break;
		}
		EXTI_clear(EXTI10);
	}
}

// tasto 'Z', 'T'
void EXTI9_5_IRQHandler(void)
{
	if (EXTI_isset(EXTI5)) { //'Z'
		if(status == ST_SETUP && timer > T_MIN){
			timer--;
		}
		else if(status == ST_RUN || status == ST_STOP){
			counter = timer;
			sprintf(s,"%2d00", counter);
			DISPLAY_puts(0, s);
		}
		EXTI_clear(EXTI5);
    }
	if (EXTI_isset(EXTI6)) { //'T'
		if(status == ST_SETUP && timer < T_MAX){
			timer++;
		}
        EXTI_clear(EXTI6);
    }
}

// tasto 'Y'
void EXTI4_IRQHandler(void)
{
	if(EXTI_isset(EXTI4)){
		GPIO_write(GPIOC, 3, 0);
		switch(status){
		case ST_IDLE:
			status = ST_RUN;
			break;
		case ST_RUN:
			status = ST_STOP;
			break;
		case ST_STOP:
			status = ST_RUN;
			break;
		}
		EXTI_clear(EXTI4);
	}
}
