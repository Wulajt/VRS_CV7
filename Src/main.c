/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <stdlib.h>


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);


/* Function processing DMA Rx data. Counts how many capital and small letters are in sentence.
 * Result is supposed to be stored in global variable of type "letter_count_" that is defined in "main.h"
 *
 * @param1 - received sign
 */
void proccesDmaData(const uint8_t* sign, uint8_t length);


/* Space for your global variables. */

	// type your global variables here:

uint8_t count = 0;
uint8_t count_big = 0;
uint8_t count_small = 0;
uint8_t state = 0;

int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();

  /* Space for your local variables, callback registration ...*/

  	  //type your code here:
  USART2_RegisterCallback(proccesDmaData);

  uint8_t *text_template = "Buffer capacity: %d bytes, occupied memory: %d bytes, load [in %%]: %3.2f%%\n\r";
  uint8_t *text_size = "Buffer capacity:  bytes, occupied memory:  bytes, load [in %]: %\n\r";

  while (1)
  {
	  /* Periodic transmission of information about DMA Rx buffer state.
	   * Transmission frequency - 5Hz.
	   * Message format - "Buffer capacity: %d bytes, occupied memory: %d bytes, load [in %]: %f%"
	   * Example message (what I wish to see in terminal) - Buffer capacity: 1000 bytes, occupied memory: 231 bytes, load [in %]: 23.1%
	   */

	  uint8_t buffer_filled = DMA_USART2_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6);
	  float buffer_percentage = (float)(100*buffer_filled)/(float)(DMA_USART2_BUFFER_SIZE);

	  uint8_t length = snprintf( NULL, 0, "%d", DMA_USART2_BUFFER_SIZE);
	  length = length + snprintf( NULL, 0, "%d", buffer_filled);
	  length = length + snprintf( NULL, 0, "%3.2f", buffer_percentage);
	  length = length + strlen(text_size);

	  uint8_t *text = (uint8_t *) malloc(length + 1);
	  snprintf(text, length + 1, text_template, DMA_USART2_BUFFER_SIZE, buffer_filled, buffer_percentage);
	  USART2_PutBuffer(text, length + 1);
	  LL_mDelay(1000);
  }
  /* USER CODE END 3 */
}


void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}

/*
 * Implementation of function processing data received via USART.
 */
void proccesDmaData(const uint8_t* sign, uint8_t length)
{
	for (uint8_t i = 0; i < length; i++){

		if(*(sign+i) == '#' || state){
			state = 1;
			if(*(sign+i) >= 'a' && *(sign+i) <= 'z'){
				count_small++;
			}

			if(*(sign+i) >= 'A' && *(sign+i) <= 'Z'){
				count_big++;
			}
			if(*(sign+i) == '\r'){
				count = i + 1;
				break;
			}
			if(*(sign+i) == '$' || (i + count + 1) >= 35){
				state = 0;
				count_small = 0;
				count_big = 0;
				count = 0;
			}
		}
	}
	return;
}


void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT

void assert_failed(char *file, uint32_t line)
{ 

}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
