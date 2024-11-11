#include "stm32l476xx.h"

#define PB4   4	//LED1 pos logic
#define PB5		5	//LED2 neg logic
#define	PC2		2	//SW1	pos logic
#define PC3		3	//SW2 neg logic


void configure_LED_pin(){
  // Enable the clock to GPIO Port B	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;   
		
	// Clear previous mode
	GPIOB->MODER &= ~(3UL<<(2*PB4));
	GPIOB->MODER &= ~(3UL<<(2*PB5));
	// Set mode to output (01)
	GPIOB->MODER |= 1UL<<(2*PB4);
	GPIOB->MODER |= 1UL<<(2*PB5);
	
	// Set output type to push-pull (0)
	GPIOB->OTYPER &= ~(1UL << PB4);
	GPIOB->OTYPER &= ~(1UL << PB5);
	
	// Clear previous push-pull state
	GPIOB->PUPDR  &= ~(3UL << (2*PB4));
	GPIOB->PUPDR  &= ~(3UL << (2*PB5));
	// Set pull-up/down
	GPIOB->PUPDR  |= 2UL <<(2*PB4);	// Pull-down (LED1)
	GPIOB->PUPDR  |= 1UL <<(2*PB5);	// Pull-up (LED2)
}


void turn_on_LED1() {
	GPIOB->ODR |= 1 << PB4;
}

void turn_on_LED2() {
	GPIOB->ODR &= ~(1 << PB5);
}

void turn_off_LED1() {
	GPIOB->ODR &= ~(1 << PB4);
}

void turn_off_LED2() {
	GPIOB->ODR |= (1 << PB5);
}

void toggle_LED1() {
	GPIOB->ODR ^= (1 << PB4);
}

void toggle_LED2() {
	GPIOB->ODR ^= (1 << PB5);
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


int button1_pressed() {
	return ((GPIOC->IDR & (1 << PC2)) == 1 << PC2);
}

int button2_pressed() {
	return ((~(GPIOC->IDR) & (1 << PC3)) == 1 << PC3);
}


void configure_EXTI() {
	// Enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	// Map EXTI line 2 & 3 to GPIOC in SYSCFG
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PC; // PC2: EXTI2
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PC; // PC3: EXTI3

	// Configure EXTI edge trigger
	EXTI->RTSR1 |= EXTI_RTSR1_RT2;	// PC2: rising edge (pos logic)
	EXTI->FTSR1 |= EXTI_FTSR1_FT3;	// PC3: falling edge (neg logic)

	// Unmask the interrupts
	EXTI->IMR1 |= EXTI_IMR1_IM2;
	EXTI->IMR1 |= EXTI_IMR1_IM3;
}


void enable_EXTI_IRQ() {
	// Enable EXTI interrupt in NVIC
	NVIC_EnableIRQ(EXTI2_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);
}

// ISR for PC2
void EXTI2_IRQHandler() {
	if (EXTI->PR1 & EXTI_PR1_PIF2) {
		toggle_LED1();

		// Clear the interrupt flag for EXTI2
		EXTI-> PR1 |= EXTI_PR1_PIF2;
	}
}

// ISR for PC3
void EXTI3_IRQHandler() {
	if (EXTI->PR1 & EXTI_PR1_PIF3) {
		toggle_LED2();

		// Clear the interrupt flag for EXTI3
		EXTI-> PR1 |= EXTI_PR1_PIF3;
	}
}


int main(void){
	configure_LED_pin();

	configure_Push_Button_pin();
	configure_EXTI();
	enable_EXTI_IRQ();

	turn_on_LED1();
	turn_on_LED2();
	while(1);
}

