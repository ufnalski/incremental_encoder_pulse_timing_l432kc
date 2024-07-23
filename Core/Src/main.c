/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <math.h>
#include "ssd1306.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define REFRESH_RATE 100 // ms

#define MICROS_PER_SLAVE_TICK 0.25
#define TICKS_PER_REVOLUTION 8

#define SAMPLING_TIME 0.01
#define TIME_CONSTANT 0.5
#define BETA_COEFF ( SAMPLING_TIME / TIME_CONSTANT )

#define UNIT_RPM
//#define UNIT_RPS

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t softTimerOLED;
char lcd_line[32];
uint16_t enc_count;
uint16_t enc_period;
double speed_avg;
double speed_filtered;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int8_t TIM_ReadEncDir(TIM_TypeDef *TIMx);
double LowPassFilter(double in_lpf);
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

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_I2C1_Init();
	MX_TIM1_Init();
	MX_TIM15_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */

	HAL_TIM_Encoder_Start_IT(&htim1, TIM_CHANNEL_ALL);

	HAL_TIM_IC_Start_IT(&htim15, TIM_CHANNEL_1);  // pulse timing

	HAL_TIM_Base_Start_IT(&htim2);  // digital filter pacing

	ssd1306_Init();
	ssd1306_Fill(Black);
	ssd1306_SetCursor(2, 0);
	ssd1306_WriteString("Encoder pulse timing", Font_6x8, White);
	ssd1306_UpdateScreen();

//	HAL_Delay(250);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	softTimerOLED = HAL_GetTick();
	while (1)
	{
		if ((HAL_GetTick() - softTimerOLED) > REFRESH_RATE)
		{
			softTimerOLED = HAL_GetTick();

			ssd1306_Fill(Black);
			ssd1306_SetCursor(2, 0);
			ssd1306_WriteString("Encoder pulse timing", Font_6x8, White);

			sprintf(lcd_line, "ENC_COUNT = %u  ", enc_count);
			ssd1306_SetCursor(5, 15);
			ssd1306_WriteString(lcd_line, Font_6x8, White);
			sprintf(lcd_line, "ENC_DIR = %i  ", TIM_ReadEncDir(TIM1));
			ssd1306_SetCursor(5, 25);
			ssd1306_WriteString(lcd_line, Font_6x8, White);
			sprintf(lcd_line, "T_PULSE = %i", enc_period);
			ssd1306_SetCursor(5, 35);
			ssd1306_WriteString(lcd_line, Font_6x8, White);

#ifdef UNIT_RPS
			sprintf(lcd_line, "SPEED = %.2f [rps]", speed_avg);
			ssd1306_SetCursor(5, 47);
			ssd1306_WriteString(lcd_line, Font_6x8, White);
			sprintf(lcd_line, "W = %.1f [rps x10000]", speed_filtered / 10000);
			ssd1306_SetCursor(5, 56);
			ssd1306_WriteString(lcd_line, Font_6x8, White);
#endif

#ifdef UNIT_RPM
			sprintf(lcd_line, "W = %.0f [rpm]", speed_avg);
			ssd1306_SetCursor(5, 47);
			ssd1306_WriteString(lcd_line, Font_6x8, White);
			sprintf(lcd_line, "W = %.1f [rpm x1000]", speed_filtered / 1000);
			ssd1306_SetCursor(5, 56);
			ssd1306_WriteString(lcd_line, Font_6x8, White);
#endif

			ssd1306_UpdateScreen();
		}
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct =
	{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
	{ 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure LSE Drive Capability
	 */
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE
			| RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 16;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Enable MSI Auto calibration
	 */
	HAL_RCCEx_EnableMSIPLLMode();
}

/* USER CODE BEGIN 4 */
int8_t TIM_ReadEncDir(TIM_TypeDef *TIMx)
{

	if (((TIMx->CR1) & (0x00000001 << 4)) == 0)  // DIR bit
	{
		return 1;
	}
	else
	{
		return -1;
	}

}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

	if (htim->Instance == TIM15)
	{
		enc_period = TIM15->CCR1;
		HAL_GPIO_TogglePin(LOGIC_ANALYZER_ENCODER_GPIO_Port,
		LOGIC_ANALYZER_ENCODER_Pin);
		HAL_GPIO_TogglePin(SUPERFLUX_LED_GPIO_Port,
		SUPERFLUX_LED_Pin);
	}

	if (htim->Instance == TIM1)
	{
		enc_count = __HAL_TIM_GET_COUNTER(&htim1);
	}

}

double LowPassFilter(double in_lpf)
{

	static double out_lpf = 0;
	if ((in_lpf == INFINITY) || (in_lpf == -INFINITY))
	{
		in_lpf = 0;
	}
	out_lpf = in_lpf * BETA_COEFF + out_lpf * (1 - BETA_COEFF);
	return out_lpf;

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if (htim->Instance == TIM2)
	{

#ifdef UNIT_RPS
		speed_avg = ((float) TIM_ReadEncDir(TIM1)) * 1000000.0
				/ TICKS_PER_REVOLUTION / MICROS_PER_SLAVE_TICK / enc_period;
#endif

#ifdef UNIT_RPM
		speed_avg = ((float) TIM_ReadEncDir(TIM1)) * 1000000.0
				/ TICKS_PER_REVOLUTION / MICROS_PER_SLAVE_TICK / enc_period
				* 60.0;
#endif

		speed_filtered = LowPassFilter(speed_avg);

	}

}
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
