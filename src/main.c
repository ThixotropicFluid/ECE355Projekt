//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: ECE 355 "Microprocessor-Based Systems".
// This is template code for Part 2 of Introductory Lab.
//
// See "system/include/cmsis/stm32f051x8.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f051x8.c" for handler declarations.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"
#include "cmsis/cmsis_device.h"

// ----------------------------------------------------------------------------
//
// STM32F0 empty sample (trace via $(trace)).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the $(trace) output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/* Definitions of registers and their bits are
   given in system/include/cmsis/stm32f051x8.h */


/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)

void myGPIOA_Init(void);
void myTIM2_Init(void);
void myEXTI_Init(void);


// Declare/initialize your global variables here...
// NOTE: You'll need at least one global variable
// (say, timerTriggered = 0 or 1) to indicate
// whether TIM2 has started counting or not.


/*** Call this function to boost the STM32F0xx clock to 48 MHz ***/

void SystemClock48MHz( void )
{
//
// Disable the PLL
//
    RCC->CR &= ~(RCC_CR_PLLON);
//
// Wait for the PLL to unlock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );
//
// Configure the PLL for 48-MHz system clock
//
    RCC->CFGR = 0x00280000;
//
// Enable the PLL
//
    RCC->CR |= RCC_CR_PLLON;
//
// Wait for the PLL to lock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );
//
// Switch the processor to the PLL clock source
//
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL;
//
// Update the system with the new clock frequency
//
    SystemCoreClockUpdate();

}

/*****************************************************************/

//
volatile int cycles;
volatile int cycles_1;
volatile int firstEdge;
volatile int firstEdge_1;

int main(int argc, char* argv[])
{

	firstEdge = 0;
	firstEdge_1 = 0;
	cycles = 0;
	cycles_1 = 0;

	SystemClock48MHz();

	trace_printf("This is Part 2 of Introductory Lab...\n");
	trace_printf("System clock: %u Hz\n", SystemCoreClock);

	myGPIOA_Init();		/* Initialize I/O port PA */
	myTIM2_Init();		/* Initialize timer TIM2 */
	myEXTI_Init();		/* Initialize EXTI */

	int active = 1;

	while (1)
	{
		while ((GPIOA->IDR & 0x1) == 0);
		trace_printf("Latch Open\n");
		if(active == 1) {
			NVIC_EnableIRQ(EXTI2_3_IRQn);
			NVIC_DisableIRQ(EXTI0_1_IRQn);
			trace_printf("Active = 2 \n");
			active = 2;
		}
		else if (active == 2) {
			NVIC_DisableIRQ(EXTI2_3_IRQn);
			NVIC_EnableIRQ(EXTI0_1_IRQn);
			trace_printf("Active = 1 \n");
			active = 1;
		}
		while ((GPIOA->IDR & 0x1) != 0);
		trace_printf("Latch Close\n");
		//trace_printf("asfsda: %d\n", GPIOA->IDR & 0x1); // read user in
	}

	return 0;

}

void myGPIOA_Init()
{
	/* Enable clock for GPIOA peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	/* Configure PA2 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER2);
	GPIOA->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1);
	/* Ensure no pull-up/pull-down for PA0 */
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR0_1 | GPIO_PUPDR_PUPDR1_1); // Set PA0 and PA1 to pull-down

}

void myTIM2_Init()
{
	/* Enable clock for TIM2 peripheral */
	// Relevant register: RCC->APB1ENR
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;


	/* Configure TIM2: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	TIM2->CR1 = ((uint16_t)0x008C);
	// Relevant register: TIM2->CR1

	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER;
	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;

	/* Update timer registers */
	// Relevant register: TIM2->EGR
	TIM2->EGR = ((uint16_t)0x0001);


	/* Assign TIM2 interrupt priority = 0 in NVIC */
	// Relevant register: NVIC->IP[3], or use NVIC_SetPriority
	NVIC_SetPriority(TIM2_IRQn, 0);


	/* Enable TIM2 interrupts in NVIC */
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable update interrupt generation */
	// Relevant register: TIM2->DIER
	TIM2->DIER |= TIM_DIER_UIE;
	//TIM2->CR1 |= TIM_CR1_CEN;
}

void myEXTI_Init()
{



	//Map EXTI2 line to PA2
	// Relevant register: SYSCFG->EXTICR[0]
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2; // Clear EXTI2 configuration
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA; // Select PA2 as the source for EXTI2

	/* Map EXTI1 line to PA1 */
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1; // Clear EXTI1 configuration
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA; // Select PA1 as the source for EXTI1

	//EXTI2 line interrupts: set rising-edge trigger
	// Relevant register: EXTI->RTSR
	EXTI->RTSR |= EXTI_RTSR_TR2;
	EXTI->RTSR |= EXTI_RTSR_TR1; // EXTI1

	//Unmask interrupts from EXTI2 line
	// Relevant register: EXTI->IMR
    EXTI->IMR |= EXTI_IMR_IM2;
    EXTI->IMR |= EXTI_IMR_IM1; // EXTI1

	//Assign EXTI2 interrupt priority = 0 in NVIC
	// Relevant register: NVIC->IP[2], or use NVIC_SetPriority
	NVIC_SetPriority(EXTI2_3_IRQn, 0);
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	//Enable EXTI2 interrupts in NVIC
	// Relevant register: NVIC->ISER[0], or use NVIC_EnableIRQ
	NVIC_DisableIRQ(EXTI2_3_IRQn);
	NVIC_EnableIRQ(EXTI0_1_IRQn);

	//NVIC_DisableIQR(EXTI2_3_IRQn);

}


/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void TIM2_IRQHandler()
{
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		// Relevant register: TIM2->SR

		/* Restart stopped timer */
		// Relevant register: TIM2->CR1
		TIM2->CNT &= 0x0;
		TIM2->SR &= ~TIM_SR_UIF;
	}
}

/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI0_1_IRQHandler() {
	if ((EXTI->PR & EXTI_PR_PR1) != 0) {

		if(firstEdge_1 == 0) {
			TIM2->CNT = 0x0;
			TIM2->CR1 |= TIM_CR1_CEN;
			cycles_1 = 0;
			firstEdge_1 ++;
		} else if(firstEdge_1 == 1) {
			cycles_1 = TIM2->CNT - 5;
			float p = ((float)cycles_1)/48.0 * 1000.0;
			int ns = p;
			float f = 48000000.0/(cycles_1);
			trace_printf("PA1: ns %d, f = %d \n", ns, (int)f);
			firstEdge_1 = 0;
		}
		EXTI->PR |= EXTI_PR_PR1; // Clear the EXTI2 pending bit

	}
	if ((EXTI->PR & EXTI_PR_PR0) != 0) {
		trace_printf("PIN 0\n");
		EXTI->PR |= EXTI_PR_PR0; // Clear the EXTI2 pending bit
	}


}
void EXTI2_3_IRQHandler()
{
	// Declare/initialize your local variables here...


	/* Check if EXTI2 interrupt pending flag is indeed set */
	if ((EXTI->PR & EXTI_PR_PR2) != 0)
	{

		if(firstEdge == 0) {
			TIM2->CNT = 0;
			TIM2->CR1 |= TIM_CR1_CEN;
			cycles = 0;
			firstEdge ++;
		} else if(firstEdge == 1) {
			cycles = TIM2->CNT - 5;
			float p = ((float)cycles)/48.0 * 1000.0;
			int ns = p;
			float f = 48000000.0/(cycles);
			trace_printf("PA2: ns %d, f = %d \n", ns, (int)f);
			firstEdge = 0;
		} else if (firstEdge == 2) {

		}
		//trace_printf("ms: %d \n", cycles/80000);
		//
		// 1. If this is the first edge:
		//	- Clear count register (TIM2->CNT).
		//	- Start timer (TIM2->CR1).
		//    Else (this is the second edge):
		//	- Stop timer (TIM2->CR1).
		//	- Read out count register (TIM2->CNT).
		//	- Calculate signal period and frequency.
		//	- Print calculated values to the console.
		//	  NOTE: Function trace_printf does not work
		//	  with floating-point numbers: you must use
		//	  "unsigned int" type to print your signal
		//	  period and frequency.
		//
		// 2. Clear EXTI2 interrupt pending flag (EXTI->PR).
		// NOTE: A pending register (PR) bit is cleared
		// by writing 1 to it.
		//

		EXTI->PR |= EXTI_PR_PR2; // Clear the EXTI2 pending bit
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
