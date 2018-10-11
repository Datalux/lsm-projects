#include "stm32_unict_lib.h"

#define MIN_TIME 	2		// il tempo minimo del timer
#define MAX_TIME 	10		// il tempo massimo del timer

#define ST_CONFIG 	0		// status di cofigurazione
#define ST_LED		1		// status di accensione del led

#define LED_ON  	1		
#define LED_OFF 	0

int status;				// stato del programma (confiugrazione o led)
int status_led;			// stato del led (acceso o spento)
int counter;			// contatore del tempo di accensione
char s_counter[5];		// stringa da scrivere sullo schermo
int timer;				// contatore del tempo trascorso

int main(){

	/* CONFIGURAZIONE GPIO LED, PULSANTI E DISPLAY */

	GPIO_init(GPIOB);
	// LED at PB0
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

	/* CONFIGURAZIONE INTERRUPT */

	GPIO_config_EXTI(GPIOB, EXTI10);
	EXTI_enable(EXTI10, FALLING_EDGE);

	GPIO_config_EXTI(GPIOB, EXTI5);
	EXTI_enable(EXTI5, FALLING_EDGE);

	GPIO_config_EXTI(GPIOB, EXTI4);
	EXTI_enable(EXTI4, FALLING_EDGE);

	GPIO_config_EXTI(GPIOB, EXTI6);
	EXTI_enable(EXTI6, FALLING_EDGE);

	/* INIZIALIZZAZIONE VARIABILI */

	status = ST_LED;		// lo stato iniziale viene impostato come accensione
	status_led = LED_ON;	// lo stato iniziale del led è acceso
	counter = MIN_TIME;		// il tempo di accensione è impostato al valore minimo
	timer = 0;				// il timer del tempo trascorso è ovviamente 0

	
	/* viene stampato sul display il tempo di accensione */

	sprintf(s_counter, "%4d", counter);
	DISPLAY_puts(0, s_counter);

	/* loop */

	while(1){
		/* viene controllato lo stato */
		switch (status) {
			case ST_LED:
				/* viene pulito il display */
				DISPLAY_puts(0, "");
				 /* se lo stato del led è acceso */
				if(status_led == LED_ON){
					/* viene atteso un tempo di un secondo */
					delay_ms(1000);
					 /* viene incrementato il timer del tempo trascorso */
					++timer;
					/* se il tempo trascorso ha raggiunto il tempo di accensione */
					if (timer == counter) { 
						/* si pone lo stato del led come spento */
						status_led = LED_OFF;
						/* si azzera il timer del tempo trascorso */
						timer = 0;
					}
					/* accendiamo il led */
					GPIO_write(GPIOB,0,1);

					/* stampiamo sullo schermo il tempo rimanente */
					sprintf(s_counter, "%4d", counter-timer);
					DISPLAY_puts(0, s_counter);
				} else if (status_led == LED_OFF){ 
					/* spegniamo il led se lo stato del led è spento */
					GPIO_write(GPIOB,0,0);

				}
				break;
			case ST_CONFIG:
				/* stampiamo sul display la schermata di configurazione */
				sprintf(s_counter, "%4d", counter);
				s_counter[0] = 'C';
				DISPLAY_puts(0, s_counter);
				break;
		}
	}

	return 0;

}

/* HANDLER DEGLI INTERRUPT */

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
