#include "stm32_unict_lib.h"

enum {
	ST_IDLE,
	ST_OPEN,
	ST_WAIT,
	ST_CLOSE
};

#define TC		20	/* 10 secondi */
#define TA		8	/* 8 secondi */

int status = ST_IDLE;

int timer_tc = 0;
int timer_ta = 0;

void main(void){

	GPIO_init(GPIOB);
	DISPLAY_init();

	GPIO_config_output(GPIOB, 0);

	GPIO_config_input(GPIOB, 10);
	GPIO_config_input(GPIOB, 5);
	GPIO_config_input(GPIOB, 4);

	GPIO_config_EXTI(GPIOB, EXTI10);
	EXTI_enable(EXTI10, FALLING_EDGE);

	GPIO_config_EXTI(GPIOB, EXTI5);
	EXTI_enable(EXTI5, FALLING_EDGE);

	GPIO_config_EXTI(GPIOB, EXTI4);
	EXTI_enable(EXTI4, FALLING_EDGE);


	TIM_init(TIM2);

	TIM_set(TIM2, 0);
	TIM_on(TIM2);

	TIM_enable_irq(TIM2, IRQ_UPDATE);

	while(1){
		switch(status){
		case ST_IDLE:
			display_puts(0, "----");
			break;
		default:
			if(timer_tc < 5)
				display_puts(0, "----");
			else if(timer_tc < 10)
				display_puts(0, "--- ");
			else if(timer_tc < 15)
				display_puts(0, "--  ");
			else if(timer_tc < 19)
				display_puts(0, "-   ");
			else
				display_puts(0, "    ");
			break;
	}



}

void TIM2_IRQHandler(void){
	if(TIM_update_check(TIM2)){
		switch(status){
		case ST_IDLE:
			GPIO_write(GPIOB, 0, 0);
			break;
		case ST_OPEN:
			GPIO_toggle(GPIOB, 0);
			++timer_tc;
			if(timer_tc >= TC){
				status = ST_IDLE;
				timer_ta = 0;
			}
			break;
		case ST_WAIT:
			GPIO_toggle(GPIOB, 0);
			++timer_ta;
			if(timer_ta >= TA){
				status = ST_CLOSE;
				timer_tc = 0;
			}
			break;
		case ST_CLOSE:
			GPIO_toggle(GPIOB, 0);
			--timer_tc;
			if(timer_tc < TC)
				status = ST_IDLE;
			break;

		}
		TIM_update_clear(TIM2);
	}
}


void EXTI15_10_IRQHandler(void){
	if(EXTI_isset(EXTI10)){
			switch(status){
			case ST_IDLE:
				status = ST_OPEN;
				break;
			case ST_CLOSE:
				status = ST_OPEN;
				break;
			}
			EXTI_clear(EXTI10);
		}
}

void EXTI9_5_IRQHandler(void){
	if(EXTI_isset(EXTI5)){
		switch(status){
		case ST_WAIT:
			timer_ta = 0;
			break;
		case ST_CLOSE:
			status = ST_OPEN;
			break;
		}
		EXTI_clear(EXTI5);
	}

}

void EXTI4_IRQHandler(void){
	if(EXTI_isset(EXTI4)){
		switch(status){
		case ST_WAIT:
			status = ST_CLOSE;
			break;
		}
		EXTI_clear(EXTI4);
	}

}
