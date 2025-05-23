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
#include "state_machine.h"
#include "filters.h"
#include "serial_handler.h"
#include "agc.h"
#include "PID.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DAC_HandleTypeDef hdac2;
SDADC_HandleTypeDef hsdadc1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim19;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint8_t programming_mode = 0;
uint8_t blink_ACQ_LED = 0;
uint8_t flag_new_sample = 0;
uint32_t gain;
uint32_t filtered_gain;
uint16_t lectura_vda;
uint16_t filtered_vda;

float setpoint = 45000;		// Lectura del ADC buscada para el algoritmo de AGC
uint16_t outPID;			// Salida de PID convertida a entero


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SDADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_ADC1_Init(void);
static void MX_DAC2_Init(void);
static void MX_TIM19_Init(void);
/* USER CODE BEGIN PFP */
uint16_t Read_ADC(SDADC_HandleTypeDef *hsdadc);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	FiltroMediaMovil filtro_potentiometer;
	FiltroMediaMovil filtro_out_PID;
	FiltroMediaMovil_Init(&filtro_potentiometer, 30);	// Filtro con ventana de tamaño 30
	FiltroMediaMovil_Init(&filtro_out_PID, 3);       // Filtro con ventana de tamaño 3

	/* Inicializo el Controlador PID*/
	PIDController pid = { PID_KP, PID_KI, PID_KD,
	PID_LIM_MIN, PID_LIM_MAX,
	PID_LIM_MIN_INT, PID_LIM_MAX_INT,
	T_MUESTREO * 0.001 };
	PIDController_Init(&pid);

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
	MX_SDADC1_Init();
	MX_USART1_UART_Init();
	MX_TIM2_Init();
	MX_ADC1_Init();
	MX_DAC2_Init();
	MX_TIM19_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim19);
	HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);
	SerialHandler_Init(&huart1);
	HAL_GPIO_WritePin(POWER_LED_GPIO_Port, POWER_LED_Pin, 1);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		if (flag_new_sample == 1) {
			if (current_state == STATE_MANUAL_OPERATION|| current_state == STATE_ACQUIRING_MANUAL) {
				HAL_ADC_Start(&hadc1);
				HAL_ADC_PollForConversion(&hadc1, 10);
				gain = HAL_ADC_GetValue(&hadc1);
				filtered_gain = (uint16_t)FiltroMediaMovil_Update(&filtro_potentiometer, gain);
				HAL_ADC_Stop(&hadc1);
				HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R,
						filtered_gain);
				lectura_vda = Read_ADC(&hsdadc1);
				filtered_vda = iir_filter(lectura_vda, num_notch, den_notch,
						FILTER_ORDER, FILTER_ORDER, x_notch, y_notch);
				SerialHandler_SendData(filtered_vda);

			}else if (current_state == STATE_AUTOMATIC_OPERATION) {

				lectura_vda = Read_ADC(&hsdadc1);
				filtered_vda = iir_filter(lectura_vda, num_notch, den_notch, FILTER_ORDER, FILTER_ORDER, x_notch, y_notch);

				// Decimacion para el AGC 180Hz -> 36Hz
				static uint8_t counter = 0;
				counter++;
				if (counter >= 5) {
					AGC_AddSample(filtered_vda);
					counter = 0;
				}

				uint16_t signal_amplitude;
				signal_amplitude = AGC_GetMax();
				outPID = (uint16_t)(PIDController_Update(&pid, setpoint, (float)signal_amplitude));
				gain =  (uint16_t)FiltroMediaMovil_Update(&filtro_out_PID, outPID);
				HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, gain);
				SerialHandler_SendData(filtered_vda);

			}else if (current_state == STATE_ACQUIRING_AUTO) {

				lectura_vda = Read_ADC(&hsdadc1);
				filtered_vda = iir_filter(lectura_vda, num_notch, den_notch, FILTER_ORDER, FILTER_ORDER, x_notch, y_notch);
				HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, gain);
				SerialHandler_SendData(filtered_vda);
			}

			flag_new_sample = 0;
		}

		if (programming_mode) {
			HAL_GPIO_WritePin(FF_D_GPIO_Port, FF_D_Pin, 1);
			HAL_GPIO_WritePin(FF_CLK_GPIO_Port, FF_CLK_Pin, 0);
			HAL_Delay(1);
			HAL_GPIO_WritePin(FF_CLK_GPIO_Port, FF_CLK_Pin, 1);
			HAL_Delay(1);
			HAL_GPIO_WritePin(FF_CLK_GPIO_Port, FF_CLK_Pin, 1);
			NVIC_SystemReset();

			//programming_mode = 0; // Reinicia el flag después de procesar
		}

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1
			| RCC_PERIPHCLK_ADC1 | RCC_PERIPHCLK_SDADC;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInit.SdadcClockSelection = RCC_SDADCSYSCLK_DIV12;
	PeriphClkInit.Adc1ClockSelection = RCC_ADC1PCLK2_DIV2;

	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
	HAL_PWREx_EnableSDADC(PWR_SDADC_ANALOG1);
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */

	/** Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_8;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief DAC2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_DAC2_Init(void) {

	/* USER CODE BEGIN DAC2_Init 0 */

	/* USER CODE END DAC2_Init 0 */

	DAC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN DAC2_Init 1 */

	/* USER CODE END DAC2_Init 1 */

	/** DAC Initialization
	 */
	hdac2.Instance = DAC2;
	if (HAL_DAC_Init(&hdac2) != HAL_OK) {
		Error_Handler();
	}

	/** DAC channel OUT1 config
	 */
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	if (HAL_DAC_ConfigChannel(&hdac2, &sConfig, DAC_CHANNEL_1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN DAC2_Init 2 */

	/* USER CODE END DAC2_Init 2 */

}

/**
 * @brief SDADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SDADC1_Init(void) {

	/* USER CODE BEGIN SDADC1_Init 0 */

	/* USER CODE END SDADC1_Init 0 */

	SDADC_ConfParamTypeDef ConfParamStruct = { 0 };

	/* USER CODE BEGIN SDADC1_Init 1 */

	/* USER CODE END SDADC1_Init 1 */

	/** Configure the SDADC low power mode, fast conversion mode,
	 slow clock mode and SDADC1 reference voltage
	 */
	hsdadc1.Instance = SDADC1;
	hsdadc1.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
	hsdadc1.Init.FastConversionMode = SDADC_FAST_CONV_DISABLE;
	hsdadc1.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
	hsdadc1.Init.ReferenceVoltage = SDADC_VREF_VREFINT2;
	hsdadc1.InjectedTrigger = SDADC_SOFTWARE_TRIGGER;
	if (HAL_SDADC_Init(&hsdadc1) != HAL_OK) {
		Error_Handler();
	}

	/** Configure the Injected Mode
	 */
	if (HAL_SDADC_SelectInjectedDelay(&hsdadc1, SDADC_INJECTED_DELAY_NONE)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_SDADC_SelectInjectedTrigger(&hsdadc1, SDADC_SOFTWARE_TRIGGER)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_SDADC_InjectedConfigChannel(&hsdadc1, SDADC_CHANNEL_5,
			SDADC_CONTINUOUS_CONV_OFF) != HAL_OK) {
		Error_Handler();
	}

	/** Set parameters for SDADC configuration 0 Register
	 */
	ConfParamStruct.InputMode = SDADC_INPUT_MODE_SE_ZERO_REFERENCE;
	ConfParamStruct.Gain = SDADC_GAIN_1_2;
	ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
	ConfParamStruct.Offset = 0;
	if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_0,
			&ConfParamStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Configure the Injected Channel
	 */
	if (HAL_SDADC_AssociateChannelConfig(&hsdadc1, SDADC_CHANNEL_5,
			SDADC_CONF_INDEX_0) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN SDADC1_Init 2 */

	/* USER CODE END SDADC1_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 7;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 49999;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief TIM19 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM19_Init(void) {

	/* USER CODE BEGIN TIM19_Init 0 */

	/* USER CODE END TIM19_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM19_Init 1 */

	/* USER CODE END TIM19_Init 1 */
	htim19.Instance = TIM19;
	htim19.Init.Prescaler = 7200 - 1;
	htim19.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim19.Init.Period = 10000 - 1;
	htim19.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim19.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim19) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim19, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim19, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM19_Init 2 */

	/* USER CODE END TIM19_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB,
			ACQ_LED_Pin | POWER_LED_Pin | FF_D_Pin | FF_CLK_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pins : SW1_Pin SW2_Pin SW3_Pin */
	GPIO_InitStruct.Pin = SW1_Pin | SW2_Pin | SW3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : ACQ_LED_Pin POWER_LED_Pin FF_D_Pin FF_CLK_Pin */
	GPIO_InitStruct.Pin = ACQ_LED_Pin | POWER_LED_Pin | FF_D_Pin | FF_CLK_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		flag_new_sample = 1;

	} else if (htim->Instance == TIM19) {
		if (blink_ACQ_LED == 1) {
			HAL_GPIO_TogglePin(ACQ_LED_GPIO_Port, ACQ_LED_Pin);
		}

	}

}

uint16_t Read_ADC(SDADC_HandleTypeDef *hsdadc) {
	int16_t adcVal;
	uint32_t channel;
	uint16_t vin;

	// Inicia la conversión inyectada
	HAL_SDADC_InjectedStart(hsdadc);

	// Espera a que la conversión se complete
	HAL_SDADC_PollForInjectedConversion(hsdadc, HAL_MAX_DELAY);

	// Obtiene el valor del ADC
	adcVal = HAL_SDADC_InjectedGetValue(hsdadc, &channel);

	// Detiene la conversión
	HAL_SDADC_InjectedStop(hsdadc);

	// Calcula el voltaje en base al valor del ADC
	vin = (adcVal + 32768);

	return vin; // Retorna el valor en voltios
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
