#include "stm32l476xx.h"

// PA5  <--> Green LED

#define PB4 4 //LED1
#define PB5 5 //LED2
#define	PC2 2 //SW1
#define PC3 3 //SW2


void configure_LED_pin(){
  // Enable the clock to GPIO Port B	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;   
		
	// Clear previous mode
	GPIOB->MODER &= ~(3UL<<(2*PB4));
	GPIOB->MODER &= ~(3UL<<(2*PB5));
	// Set mode to output (01)
	GPIOB->MODER |= 1UL<<(2*PB4);
	GPIOB->MODER |= 1UL<<(2*PB5);
	
	// Set output type to open drain (1)
	GPIOB->OTYPER &= ~(1UL << PB4);
	GPIOB->OTYPER &= ~(1UL << PB5);
	
	// Clear previous push-pull state
	GPIOB->PUPDR  &= ~(3UL << (2*PB4));
	GPIOB->PUPDR  &= ~(3UL << (2*PB5));
	// Set pull-up/down
	GPIOB->PUPDR  |= 2UL <<(2*PB4);	// Pull-down (LED1)
	GPIOB->PUPDR  |= 1UL <<(2*PB5);	// Pull-up (LED2)
}

void configure_Push_Button_pin(){
  // 1. Enable the clock to GPIO Port A	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;   
		
	// 2. Configure GPIO Mode to 'Input': Input(00), Output(01), AlterFunc(10), Analog(11)
	GPIOC->MODER &= ~(3UL << (2*PC2));  	// Input (00)
	GPIOC->MODER &= ~(3UL << (2*PC3));  	// Input (00)
	
	// 3. Configure GPIO Push-Pull to 'No Pull-up or Pull-down': No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOC->PUPDR  &= ~(3UL << (2*PC2));  	// Reset
	GPIOC->PUPDR  &= ~(3UL << (2*PC3));  	// Reset
	
	GPIOC->PUPDR  |= 2UL << (2*PC2);  		// Pull-down, Positive logic (SW1)
	GPIOC->PUPDR  |= 1UL << (2*PC3);  		// Pull-up, Negative logic (SW2)
}

// Modular function to turn on the LD2 LED.
void turn_on_LED1(){
	GPIOB->ODR |= 1 << PB4;
}
void turn_on_LED2(){
	GPIOB->ODR &= ~(1 << PB5);
}

// Modular function to turn off the LD2 LED.
void turn_off_LED1(){
	GPIOB->ODR &= ~(1 << PB4);
}
void turn_off_LED2(){
	GPIOB->ODR |= (1 << PB5);
}

// Modular function to toggle the LD2 LED.
void toggle_LED1(){
	GPIOB->ODR ^= (1 << PB4);
}
void toggle_LED2(){
	GPIOB->ODR ^= (1 << PB5);
}

int main(void){
	configure_LED_pin();
	configure_Push_Button_pin();
	
	// Flash LEDs to confirm set up.
	turn_on_LED1();
	turn_on_LED2();
	int i;
	for (i = 0; i < 100000; i++);
	turn_off_LED1();
	turn_off_LED2();
	
	while(1){
		for (i = 0; i < 100000; i++);		
			if ((~(GPIOC->IDR) & (1 << PC3)) == 1 << PC3) {	// If SW2 is pressed
				turn_off_LED1();
				turn_off_LED2();
			}
			else if ((GPIOC->IDR & (1 << PC2)) == 1 << PC2) {	// If SW1 is pressed
				toggle_LED1();
				turn_off_LED2();
			}
			else {
				turn_off_LED1();
				toggle_LED2();
			}
	}
}

