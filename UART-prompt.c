#include "stm32_unict_lib.h"
#include <stdio.h>

int getstring(char *s);

int main(void)
{
	CONSOLE_init();
	DISPLAY_init();
	GPIO_init(GPIOB);
	GPIO_config_alternate(GPIOB, 0, 2);
	TIM_init(TIM3);

	TIM_config_timebase(TIM3, 20000, 858);
	TIM_config_PWM(TIM3, 3);
	TIM_set(TIM3, 0);
	TIM_on(TIM3);

	char string[50];

	for (;;) {
		__io_putchar('>');
		__io_putchar(32);
		int dim = getstring(string);
		if(dim == 1){
			if(*string >= 49 && *string <= 57){
				TIM_set(TIM3, 0);
				TIM_on(TIM3);
				TIM3->CCR3 = ((*string)-48)*100;
			} else {
				printf("\nErrore. Inserire un numero compreso tra 1 e 9");
			}
		} else {
			printf("\nErrore. Inserire solo un numero");
		}
	}
}

int getstring(char *s){
	int index = 0;
	while(1){
		if(kbhit()){
			char c = readchar();
			if(c == 8 && index > 0){
				index--;
				s[index] = '\0';
				__io_putchar(8);
				__io_putchar(32);
				__io_putchar(8);
			} else if(c == 13){
				if(index == 0){
					__io_putchar('\n');
					return 0;
				}
				__io_putchar('\n');
				return index;
			}
			else if(index < 50){
				__io_putchar(c);
				s[index] = c;
				s[++index] = '\0';
			}
		}
	}
}



