#include "stm32_unict_lib.h"

#define MIN_TIME 	2
#define MAX_TIME 	10
#define ST_CONFIG 	0
#define ST_LED		1

#define LED_ON  	1
#define LED_OFF 	0

int status; // the status (config, led)
int status_led;
int counter;
char s_counter[5];
int timer;

int main(){
	// LED at PB0
	GPIO_init(GPIOB);
	GPIO_config_output(GPIOB, 0);
	// pushbutton X (PB10)
	GPIO_config_input(GPIOB, 10);
	// pushbutton Y (PB4)
	GPIO_config_input(GPIOB, 4);
	// pushbutton Z (PB5)
	GPIO_config_input(GPIOB, 5);
	// pushbutton T (PB6)
	GPIO_config_input(GPIOB, 6);
	// display init
	DISPLAY_init();


	GPIO_config_EXTI(GPIOB, EXTI10);
	EXTI_enable(EXTI10, FALLING_EDGE);

	GPIO_config_EXTI(GPIOB, EXTI5);
	EXTI_enable(EXTI5, FALLING_EDGE);

	GPIO_config_EXTI(GPIOB, EXTI4);
	EXTI_enable(EXTI4, FALLING_EDGE);

	GPIO_config_EXTI(GPIOB, EXTI6);
	EXTI_enable(EXTI6, FALLING_EDGE);

	status = ST_LED;
	status_led = LED_ON;
	counter = MIN_TIME;
	timer = 0;

	sprintf(s_counter, "%4d", counter);
	DISPLAY_puts(0, s_counter);

	while(1){
	switch (status) {
		case ST_LED:
			DISPLAY_puts(0, "");
			if(status_led == LED_ON){

				delay_ms(1000);
				++timer;
				if (timer == counter) {
					status_led = LED_OFF;
					timer = 0;
				}
				GPIO_write(GPIOB,0,1);
				sprintf(s_counter, "%4d", counter-timer);
				DISPLAY_puts(0, s_counter);
			} else if (status_led == LED_OFF){
				GPIO_write(GPIOB,0,0);

			}
			break;
		case ST_CONFIG:
			sprintf(s_counter, "%4d", counter);
			s_counter[0] = 'C';
			DISPLAY_puts(0, s_counter);
			break;
		}
	}

	return 0;

}

void EXTI4_IRQHandler(void){
	if (EXTI_isset(EXTI4)) {
		GPIO_write(GPIOB,0, 0);
		status = ST_CONFIG;
		DISPLAY_puts(0, " CFG");
		EXTI_clear(EXTI4);
	}
}

void EXTI9_5_IRQHandler(void){
	if (EXTI_isset(EXTI5)) {
		if(status == ST_CONFIG && counter < MAX_TIME)
			counter++;

	EXTI_clear(EXTI5);
	}
	if (EXTI_isset(EXTI6)) {
		if(status == ST_CONFIG && counter > MIN_TIME)
			counter--;

	EXTI_clear(EXTI6);
	}
}

void EXTI15_10_IRQHandler(void){
	if (EXTI_isset(EXTI10)) {
		status = ST_LED;
		status_led = LED_ON;
		EXTI_clear(EXTI10);
	}
}
