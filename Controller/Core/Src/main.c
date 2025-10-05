/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "lcd_stm32f4.h"
/* USER CODE END Includes */
#include <string.h>
#include <stdlib.h>


/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim2_ch1;
UART_HandleTypeDef huart1;

// TODO: Equation to calculate TIM2_Ticks


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void UART1_Init(void);
void ADC1_Init(void);
void ADC1_Start(uint8_t channel);
void ADC_IRQHandler(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void handle(char* Msg);

volatile uint16_t pot0_value = 0;
volatile uint16_t pot1_value = 0;
volatile uint16_t old_pot0_value = 0;
volatile uint16_t old_pot1_value = 0;
/* USER CODE BEGIN PFP */

#define RX_BUFFER_SIZE 50
#define RX_SIZE 50
uint8_t rxBuffer[RX_BUFFER_SIZE];
uint8_t rxIndex = 0;
uint8_t rxByte;             // Temporary single-byte storage
volatile uint8_t stringReady = 0;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  UART1_Init();
  ADC1_Init();
  ADC1_Start( 5);
  
  HAL_UART_Receive_IT(&huart1, &rxByte, 1);

  init_LCD();
  lcd_command(CLEAR);
  
  char buf[10];
  

  while (1)
  {

    //Checks For pots rotations
    if ( abs(pot0_value - old_pot0_value) > 30){
        old_pot0_value = pot0_value;
        
        sprintf(buf, "POT0 %4u\r\n", pot0_value);

        HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
    }

    if ( abs(pot1_value - old_pot1_value) > 30){
        old_pot1_value = pot1_value;

        sprintf(buf, "POT1 %4u\r\n",pot1_value);
        HAL_UART_Transmit(&huart1, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
    }

    if (stringReady)
        {
            stringReady = 0; 
            char * Recieved =  (char*)rxBuffer;
            lcd_putstring(Recieved);
            handle(Recieved);
        }
    
  
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

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



/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  // -------------------------------
  // LCD pins configuration
  // -------------------------------
  // Configure PC14 (RS) and PC15 (E) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Configure PB8 (D4) and PB9 (D5) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Configure PA12 (D6) and PA15 (D7) as output push-pull
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Set all LCD pins LOW initially
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12 | GPIO_PIN_15, GPIO_PIN_RESET);


  // -------------------------------
  // Button0 configuration (PA0)
  // -------------------------------
  GPIO_InitStruct.Pin = Button0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // Interrupt on rising edge
  GPIO_InitStruct.Pull = GPIO_PULLUP;         // Use pull-up resistor
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Enable and set EXTI line 0 interrupt priority
  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  // Configure PC0, PC1, PC2, PC3 as inputs with interrupts
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;  // Interrupt on falling edge (button press)
  GPIO_InitStruct.Pull = GPIO_PULLUP;            // Enable internal pull-up resistor
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
  // Enable and set EXTI line interrupts
  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);  // SW0 on PC0
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    
  HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);  // SW1 on PC1
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    
  HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);  // SW2 on PC2
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    
  HAL_NVIC_SetPriority(EXTI3_IRQn, 2, 0);  // SW3 on PC3
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

   // Set PA5 and PA6 as analog mode, no pull-up/down
  GPIOA->MODER |= (3U << (5 * 2)) | (3U << (6 * 2));
  GPIOA->PUPDR &= ~((3U << (5 * 2)) | (3U << (6 * 2)));

  GPIOB->MODER = 0x5555;
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

uint32_t lastInterruptTime[4] = {0};
#define DEBOUNCE_DELAY 200  // 200ms debounce

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t currentTime = HAL_GetTick();
    
    switch(GPIO_Pin)
    {
        case GPIO_PIN_0:  // SW0
            if((currentTime - lastInterruptTime[0]) > DEBOUNCE_DELAY)
            {
                lastInterruptTime[3] = currentTime;
                char msg[] = "BTN0\r\n";
                HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
            }
            break;
            
        case GPIO_PIN_1:  // SW1
            if((currentTime - lastInterruptTime[1]) > DEBOUNCE_DELAY)
            {
                lastInterruptTime[3] = currentTime;
                char msg[] = "BTN1\r\n";
                HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
            }
            break;
            
        case GPIO_PIN_2:  // SW2
            if((currentTime - lastInterruptTime[2]) > DEBOUNCE_DELAY)
            {
                lastInterruptTime[3] = currentTime;
                char msg[] = "BTN2\r\n";
                HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
            }
            break;
            
        case GPIO_PIN_3:  // SW3
            if((currentTime - lastInterruptTime[3]) > DEBOUNCE_DELAY)
            {
                lastInterruptTime[3] = currentTime;
                char msg[] = "BTN3\r\n";
                HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
            }
            break;
    }
}

void ADC1_Start(uint8_t channel)
{
    ADC1->SQR3 = channel & 0x1F;                  // Select ADC channel
    ADC1->CR2 |= ADC_CR2_SWSTART;                 // Start conversion
}

void UART1_Init(void)
{
    // Enable clocks
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configure GPIO pins for UART
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // PA9 -> USART1_TX
    // PA10 -> USART1_RX
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // Configure UART parameters
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        // Initialization Error
        Error_Handler();
    }
}
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}
void ADC_IRQHandler(void)
{
    if (ADC1->SR & ADC_SR_EOC)
    {
        static uint8_t current_channel = 5;       // Start from channel 5
        uint16_t value = ADC1->DR;                // Read result, clears EOC flag

        if (current_channel == 5)
        {
            pot0_value = value;
            current_channel = 6;
            ADC1_Start(6);                        // Next: PA6
        }
        else
        {
            pot1_value = value;
            current_channel = 5; 
            ADC1_Start(5);                        // Next: PA5
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (rxByte == '\n' || rxByte == '\r')
        {
            rxBuffer[rxIndex] = '\0';
            rxIndex = 0;
            stringReady = 1;
        }
        else
        {
            if (rxIndex < RX_SIZE - 1)
                rxBuffer[rxIndex++] = rxByte;
        }

        // Receive next byte
        HAL_UART_Receive_IT(&huart1, &rxByte, 1);
    }
}

void ADC1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;           // Enable ADC1 clock

    // ADC common prescaler (PCLK2 / 4)
    ADC->CCR &= ~(3U << 16);
    ADC->CCR |= (1U << 16);                       // Divide by 4

    ADC1->CR1 = 0;                                // 12-bit, single conversion
    ADC1->CR2 = ADC_CR2_EOCS | ADC_CR2_CONT;      // EOC after each, continuous mode
    ADC1->SQR1 = 0;                               // One conversion per sequence

    // Sampling time for channels 5 and 6: 56 cycles (enough for stable reads)
    ADC1->SMPR2 |= (3U << (5 * 3)) | (3U << (6 * 3));

    // Enable EOC interrupt
    ADC1->CR1 |= ADC_CR1_EOCIE;
    NVIC_EnableIRQ(ADC_IRQn);

    ADC1->CR2 |= ADC_CR2_ADON;                    // Power on ADC
}



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

void handle(char* Msg){

  if ( (Msg[0] == 'H') && (Msg[1] == 'I') ){
    char msg[] = "HEY\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  }

  if ( (Msg[0] == 'U') && (Msg[1] == 'P') ){
    char msg[] = "YES\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  }

  
  if ( (Msg[0] == 'W') && (Msg[1] == 'R') ){
    char msg[] = "DID\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  }

  if ( (Msg[0] == 'L') && (Msg[1] == 'I') ){
    char msg[] = "LIT\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  }

  if ( (Msg[0] == 'E') && (Msg[1] == 'S') ){
    char msg[] = "SHO\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
  }



}

#ifdef USE_FULL_ASSERT
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
