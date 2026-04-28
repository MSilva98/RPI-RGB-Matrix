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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Driver_RGBMatrix.h"
#include "GUI_Paint.h"
#include <stdbool.h>
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

/* USER CODE BEGIN PV */
static UWORD gui_framebuffer[HUB75_PANEL_WIDTH * HUB75_PANEL_HEIGHT];

#define UI_BASE_WIDTH   (64u)
#define UI_BASE_HEIGHT  (32u)
#define UI_OFFSET_X     ((HUB75_PANEL_WIDTH  - UI_BASE_WIDTH) / 2u)
#define UI_OFFSET_Y     ((HUB75_PANEL_HEIGHT - UI_BASE_HEIGHT) / 2u)
#define UI_X(x)         ((UWORD)((x) + UI_OFFSET_X))
#define UI_Y(y)         ((UWORD)((y) + UI_OFFSET_Y))
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static UWORD App_Wheel(UWORD value)
{
  UWORD index = (UWORD)(value % 96U);
  UWORD section = (UWORD)(index / 32U);
  UWORD offset = (UWORD)(index % 32U);
  UWORD r = 0U;
  UWORD g = 0U;
  UWORD b = 0U;

  switch (section)
  {
    case 0U:
      r = (UWORD)(31U - offset);
      g = offset;
      break;
    case 1U:
      g = (UWORD)(31U - offset);
      b = offset;
      break;
    default:
      b = (UWORD)(31U - offset);
      r = offset;
      break;
  }

  return (UWORD)((r << 11) | ((g << 1) << 5) | b);
}

static void App_DrawPage(UWORD page)
{
  Paint_Clear(BLACK);

  switch (page)
  {
    case 0U:
      Paint_DrawPoint(UI_X(44), UI_Y(9), WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
      Paint_DrawPoint(UI_X(47), UI_Y(8), WHITE, DOT_PIXEL_2X2, DOT_STYLE_DFT);
      Paint_DrawPoint(UI_X(52), UI_Y(7), WHITE, DOT_PIXEL_3X3, DOT_STYLE_DFT);
      Paint_DrawPoint(UI_X(59), UI_Y(6), WHITE, DOT_PIXEL_4X4, DOT_STYLE_DFT);

      Paint_DrawLine(UI_X(20), UI_Y(11), UI_X(20), UI_Y(31), App_Wheel(1), DOT_PIXEL_1X1, LINE_STYLE_SOLID);
      Paint_DrawLine(UI_X(10), UI_Y(21), UI_X(30), UI_Y(21), App_Wheel(31), DOT_PIXEL_1X1, LINE_STYLE_SOLID);
      Paint_DrawCircle(UI_X(20), UI_Y(21), 10, App_Wheel(14), DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      Paint_DrawPoint(UI_X(20), UI_Y(21), (UWORD)(App_Wheel(1) | App_Wheel(31)), DOT_PIXEL_1X1, DOT_FILL_RIGHTUP);

      Paint_DrawLine(UI_X(44), UI_Y(11), UI_X(44), UI_Y(31), App_Wheel(16), DOT_PIXEL_1X1, LINE_STYLE_SOLID);
      Paint_DrawLine(UI_X(34), UI_Y(21), UI_X(54), UI_Y(21), App_Wheel(63), DOT_PIXEL_1X1, LINE_STYLE_SOLID);
      Paint_DrawCircle(UI_X(44), UI_Y(21), 10, App_Wheel(54), DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      Paint_DrawPoint(UI_X(44), UI_Y(21), (UWORD)(App_Wheel(16) | App_Wheel(63)), DOT_PIXEL_1X1, DOT_FILL_RIGHTUP);

      Paint_DrawRectangle(UI_X(1), UI_Y(1), UI_X(64), UI_Y(32), App_Wheel(6), DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
      break;
    case 1U:
      Paint_DrawString_EN(UI_X(5), UI_Y(0), "Hello", &Font16, BLACK, YELLOW);
      Paint_DrawString_EN(UI_X(5), UI_Y(15), "world", &Font12, MAGENTA, BLACK);
      break;
    default:
      Paint_DrawString_EN(UI_X(1), UI_Y(0), "Page3", &Font12, App_Wheel(35), BLACK);
      Paint_DrawNum(UI_X(1), UI_Y(20), 23.456789, &Font12, 2, App_Wheel(90), BLACK);
      break;
  }

   HUB75_LoadRGB565Frame(gui_framebuffer, HUB75_PANEL_WIDTH, HUB75_PANEL_HEIGHT);
}

static void App_WaitMsWithRefresh(uint32_t wait_ms)
{
  uint32_t t0 = HAL_GetTick();
  while ((HAL_GetTick() - t0) < wait_ms)
  {
    HUB75_Display();
  }
}

void App_DrawGuiScreen()
{
  Paint_NewImage(gui_framebuffer, HUB75_PANEL_WIDTH, HUB75_PANEL_HEIGHT, ROTATE_0, BLACK);
  Paint_SelectImage(gui_framebuffer);
  App_DrawPage(0U);
}

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
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	
  DWT_Init();
  HUB75_Init();
  HUB75_SetBrightness(128); // the parameter is 0-255，the higher the number，the brighter the screen is
  HUB75_SetRefreshRate(1); // the parameter is 1-4，the higher the number，the lower the refresh rate is
  App_DrawGuiScreen();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  UWORD page = 0U;
  while (true)
  {
    App_WaitMsWithRefresh(1000U);
    page = (UWORD)((page + 1U) % 3U);
    App_DrawPage(page);
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
