/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"


#define HW1		1


void SystemClock_Config(void);


//TIMER 2 w/ Interrupt
#if HW1
void TIM2_init(void){	// Init Timer 2 s Interruptem

	RCC->APB1ENR |= 0x01;	// povoleni hodin do TIM2 přes hodnotu
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;	// povoleni hodin do TIM2 přes makro

	TIM2->PSC = 16000 - 1; // Default 1600 - 1 (1s)
	TIM2->ARR = 100 - 1; // Default 1000 - 1 (1s)

	TIM2->DIER |= TIM_DIER_UIE; // povolení IRQ interrupt
	NVIC->ISER[0] |= 1 << 28; // povoleni TIM2 v NVIC (pozice 28 z tabulky vectoru preruseni)
	// stejne jako NVIC->EnableIRQ(28);
	// nebo jako NVIC->EnableIRQ(TIM2_IRQn);

	TIM2->CR1 = TIM_CR1_CEN;	// Spusteni TIM2


}
void TIM2_IRQHandler(void){	// Timer 2 Interrupt Handler
	TIM2->SR &= ~TIM_SR_UIF;
	GPIOA->ODR ^= 0x01 << 5;
}
#endif

//LED
#if	HW1
void LED_init(void){
	  //Init for LED
	  RCC->AHB1ENR 	|= 0x01; 		// Aktivace hodin pro GPIOA
	  GPIOA->MODER 	|= 0x01 << 10; 	// Nastavení MODER PA5 jako vystup
	  GPIOA->ODR	|= 0x01 << 5;	// rozsvitime LD2 (PA5 = 01)
}
#endif

//PushButton (USER)
#if HW1
void PushButton_init(void){
	  //Init for PushButton
	  RCC->AHB1ENR 	|= 0x01 << 2;	// Aktivace hodin pro GPIOC
	  GPIOC->MODER	&= ~(3 << 26);  // Nastaveni MODER PC13 jako vstup

	  GPIOC->PUPDR &= ~(3 << (26)); // Clear PUPDR bits for PC13
	  GPIOC->PUPDR |= (2 << (26));  // Set PUPDR bits to pull-down
//	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
}
#endif

#if HW1
//Timer Toggle
void TIM2_Toggle(void){
	if (TIM2->ARR == 99){
		TIM2->ARR = 499;
	}else if(TIM2->ARR == 499){
		TIM2->ARR = 1500;
	}else{
		TIM2->ARR = 99;
	}
	TIM2->CNT = 0;
}
#endif


int main(void)
{

	/// INIT
  HAL_Init();
  SystemClock_Config();

  TIM2_init();

  LED_init();
  PushButton_init();



  while (1)
  {

	  if (TIM2->SR & TIM_SR_UIF){

			TIM2->SR &= ~TIM_SR_UIF;
			GPIOA->ODR ^= 0x01 << 5;

	  }
	  if(!(GPIOC->IDR & GPIO_IDR_ID13)){

		  TIM2_Toggle();

	  }

  }
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}



void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
