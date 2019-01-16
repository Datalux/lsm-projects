#include "stm32_unict_lib.h"
#include <stdio.h>

#define S_IDLE 				0
#define S_UPDATE_TICKET 	1


int tickets_counter;
int current_ticket;
int estimated_time;
int timer;
int status;



int main()
{
	DISPLAY_init();

	GPIO_init(GPIOB);

	GPIO_config_output(GPIOB, 0);

	GPIO_config_input(GPIOB, 4);	/* tasto Y */
	GPIO_config_input(GPIOB, 10);	/* tasto X */

	GPIO_config_EXTI(GPIOB, EXTI4);
	EXTI_enable(EXTI4, FALLING_EDGE);

	GPIO_config_EXTI(GPIOB, EXTI10);
	EXTI_enable(EXTI10, FALLING_EDGE);




	status = tickets_counter = current_ticket = estimated_time = timer = 0;

	for (;;) {
		switch(status){
		case S_IDLE:
		{
			char s[5];
			sprintf(s, "%02d", current_ticket);
			DISPLAY_puts(0, s);
			sprintf(s, "%02d", estimated_time);
			DISPLAY_puts(2, s);
			break;
		}
		case S_UPDATE_TICKET:

			GPIO_write(GPIOB, 0, 1);

			char s[5];
			sprintf(s, "%02d", tickets_counter);
			DISPLAY_puts(0, s);
			delay_ms(500);
			DISPLAY_puts(0, "  ");
			delay_ms(500);
			DISPLAY_puts(0, s);
			delay_ms(500);
			DISPLAY_puts(0, "  ");
			delay_ms(500);

			GPIO_write(GPIOB, 0, 0);

			status = S_IDLE;

			break;

		}

	}
	return 0;
}

void EXTI4_IRQHandler(void)
{
	if (EXTI_isset(EXTI4)) {
		current_ticket++;

		EXTI_clear(EXTI4);
	}
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI_isset(EXTI10)) {
		tickets_counter++;
		status = S_UPDATE_TICKET;

		EXTI_clear(EXTI10);
	}
}



