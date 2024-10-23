
#include "main.h"

#define TIM2_IRQ	0
#define Pushbutton	1

void SystemClock_Config(void);

#if !TIM2_IRQ

void TIM2_init(void){
	RCC->APB1ENR |= 0x01;	// povoleni hodin do TIM2 přes hodnotu
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;	// povoleni hodin do TIM2 přes makro

	TIM2->PSC = 16000 - 1;
	TIM2->ARR = 500 / 1;

	TIM2->CR1 = TIM_CR1_CEN;	// Spusteni TIM2
}

#else

void TIM2_initIRQ(void){
	RCC->APB1ENR |= 0x01;	// povoleni hodin do TIM2 přes hodnotu
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;	// povoleni hodin do TIM2 přes makro

	TIM2->PSC = 16000 - 1; // Default 1600 - 1
	TIM2->ARR = 500 - 1; // Default 1000 - 1

	TIM2->DIER |= TIM_DIER_UIE; // povolení IRQ interrupt
	NVIC->ISER[0] |= 1 << 28; // povoleni TIM2 v NVIC (pozice 28 z tabulky vecotru preruseni)
	// stejne jako NVIC->EnableIRQ(28);
	// nebo jako NVIC->EnableIRQ(TIM2_IRQn);

	TIM2->CR1 = TIM_CR1_CEN;	// Spusteni TIM2


}

void TIM2_IRQHandler(void){
	TIM2->SR &= ~TIM_SR_UIF;
	GPIOA->ODR ^= 0x01 << 5;
}

#endif

int main(void)
{

  HAL_Init();
  SystemClock_Config();

  //Init for TIM2
#if TIM2_IRQ
  TIM2_initIRQ();
#else
  TIM2_init();
#endif

  //Init for LED
  RCC->AHB1ENR 	|= 0x01; 		// Aktivace hodin pro GPIOA
  GPIOA->MODER 	|= 0x01 << 10; 	// Nastavení MODER PA5 jako vystup
  GPIOA->ODR	|= 0x01 << 5;	// rozsvitime LD2 (PA5 = 01)

#if Pushbutton
  //Init for PushButton
  RCC->AHB1ENR 	|= 0x01 << 2;	// Aktivace hodin pro GPIOC
  GPIOC->MODER	&= ~(3 << 26);  // Nastaveni MODER PC13 jako vstup

  GPIOC->PUPDR &= ~(3 << (26)); // Clear PUPDR bits for PC13
  GPIOC->PUPDR |= (2 << (26));  // Set PUPDR bits to pull-down

  // Inner Variables
  int depressed = 0;

#endif

  while (1)
  {

#if Pushbutton

	  if(GPIOC->IDR & GPIO_IDR_ID13){
		  GPIOA->ODR &= ~1 << 5;
	  }else{
		  GPIOA->ODR |= 1 << 5;
	  }



#endif

//	  if (TIM2->SR & TIM_SR_UIF){
//		  TIM2->SR &= ~TIM_SR_UIF;
//		  GPIOA->ODR ^= 0x01 << 5;
//	  }

//	  GPIOA->ODR ^= 0x01 << 5;
//	  int i = 0;
//	  while (i < 500000) // Timer
//	  {
//		  i++;
//	  }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
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
