#ifndef _DRIVER_RGBMATRIX_H_
#define _DRIVER_RGBMATRIX_H_

#include "stm32f1xx_hal.h"
#include "main.h"
#include "usart.h"
#include "tim.h"
#include <stdint.h>
#include <stdlib.h>
#include "image.h"
#include <string.h>
#include "fonts.h"

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

/* hub75 interface definition */
#define HUB75_PANEL_PROFILE_64X32_1_16 (1u)
#define HUB75_PANEL_PROFILE_64X64_1_32 (2u)
#define HUB75_PANEL_PROFILE_80X40_1_20 (3u)
#define HUB75_PANEL_PROFILE_96X48_1_24 (4u)

#ifndef HUB75_PANEL_PROFILE
#define HUB75_PANEL_PROFILE HUB75_PANEL_PROFILE_80X40_1_20 
#endif

#if (HUB75_PANEL_PROFILE == HUB75_PANEL_PROFILE_64X32_1_16)
#define HUB75_PANEL_WIDTH        (64u)
#define HUB75_PANEL_HEIGHT       (32u)
#define HUB75_SCAN_ROWS_DEFAULT  (16u)
#elif (HUB75_PANEL_PROFILE == HUB75_PANEL_PROFILE_64X64_1_32)
#define HUB75_PANEL_WIDTH        (64u)
#define HUB75_PANEL_HEIGHT       (64u)
#define HUB75_SCAN_ROWS_DEFAULT  (32u)
#elif (HUB75_PANEL_PROFILE == HUB75_PANEL_PROFILE_80X40_1_20)
#define HUB75_PANEL_WIDTH        (80u)
#define HUB75_PANEL_HEIGHT       (40u)
#define HUB75_SCAN_ROWS_DEFAULT  (20u)
#elif (HUB75_PANEL_PROFILE == HUB75_PANEL_PROFILE_96X48_1_24)
#define HUB75_PANEL_WIDTH        (96u)
#define HUB75_PANEL_HEIGHT       (48u)
#define HUB75_SCAN_ROWS_DEFAULT  (24u)
#else
#error "Unsupported HUB75_PANEL_PROFILE"
#endif

#ifndef HUB75_SCAN_ROWS
#define HUB75_SCAN_ROWS          (HUB75_SCAN_ROWS_DEFAULT)
#endif
#ifndef HUB75_MAP_SCAN_ROW
#define HUB75_MAP_SCAN_ROW(row)  (row)
#endif

#define HUB75_OE_H()             (OE_GPIO_Port->BSRR = OE_Pin)
#define HUB75_OE_L()             (OE_GPIO_Port->BSRR = (uint32_t)OE_Pin << 16u)
#define HUB75_CLK_H()            (CLK_GPIO_Port->BSRR = CLK_Pin)
#define HUB75_CLK_L()            (CLK_GPIO_Port->BSRR = (uint32_t)CLK_Pin << 16u)
#define HUB75_LAT_H()            (LAT_GPIO_Port->BSRR = LAT_Pin)
#define HUB75_LAT_L()            (LAT_GPIO_Port->BSRR = (uint32_t)LAT_Pin << 16u)
#define HUB75_DR1_H()            (R1_GPIO_Port->BSRR = R1_Pin)
#define HUB75_DR1_L()            (R1_GPIO_Port->BSRR = (uint32_t)R1_Pin << 16u)
#define HUB75_DG1_H()            (G1_GPIO_Port->BSRR = G1_Pin)
#define HUB75_DG1_L()            (G1_GPIO_Port->BSRR = (uint32_t)G1_Pin << 16u)
#define HUB75_DB1_H()            (B1_GPIO_Port->BSRR = B1_Pin)
#define HUB75_DB1_L()            (B1_GPIO_Port->BSRR = (uint32_t)B1_Pin << 16u)
#define HUB75_DR2_H()            (R2_GPIO_Port->BSRR = R2_Pin)
#define HUB75_DR2_L()            (R2_GPIO_Port->BSRR = (uint32_t)R2_Pin << 16u)
#define HUB75_DG2_H()            (G2_GPIO_Port->BSRR = G2_Pin)
#define HUB75_DG2_L()            (G2_GPIO_Port->BSRR = (uint32_t)G2_Pin << 16u)
#define HUB75_DB2_H()            (B2_GPIO_Port->BSRR = B2_Pin)
#define HUB75_DB2_L()            (B2_GPIO_Port->BSRR = (uint32_t)B2_Pin << 16u)
#define HUB75_A_H()              (A_GPIO_Port->BSRR = A_Pin)
#define HUB75_A_L()              (A_GPIO_Port->BSRR = (uint32_t)A_Pin << 16u)
#define HUB75_B_H()              (B_GPIO_Port->BSRR = B_Pin)
#define HUB75_B_L()              (B_GPIO_Port->BSRR = (uint32_t)B_Pin << 16u)
#define HUB75_C_H()              (C_GPIO_Port->BSRR = C_Pin)
#define HUB75_C_L()              (C_GPIO_Port->BSRR = (uint32_t)C_Pin << 16u)
#define HUB75_D_H()              (D_GPIO_Port->BSRR = D_Pin)
#define HUB75_D_L()              (D_GPIO_Port->BSRR = (uint32_t)D_Pin << 16u)
#define HUB75_E_H()              (E_GPIO_Port->BSRR = E_Pin)
#define HUB75_E_L()              (E_GPIO_Port->BSRR = (uint32_t)E_Pin << 16u)

/* hub75 color definition */
typedef enum
{
  HUB75_Color_Black       = 00U,
  HUB75_Color_Red         = 01U,
  HUB75_Color_Green       = 02U,
  HUB75_Color_Yellow      = 03U,
  HUB75_Color_Blue        = 04U,
  HUB75_Color_Pink        = 05U,
  HUB75_Color_Cyan        = 06U,
  HUB75_Color_White       = 07U
} HUB75_ColorTypeDef;

typedef struct
{
  UWORD width;
  UWORD height;
  UBYTE address_size;
  UBYTE bitDepth;
  UWORD *BlackImage;
} HUB75_port;

extern uint8_t hub75_buff[HUB75_PANEL_WIDTH/8 * HUB75_PANEL_HEIGHT];
extern uint8_t hub75_panel_buff[HUB75_PANEL_WIDTH * HUB75_PANEL_HEIGHT / 2 * 3];
extern uint8_t hub75_color;
extern uint8_t hub75_blink;
extern HUB75_port RGB_Matrix;
extern volatile uint16_t display_tick;

void HUB75_Init(void);
void HUB75_Display(void);
void HUB75_WriteByte(uint8_t p_buff[], uint8_t color);
void HUB75_WritePixel(uint8_t p_buff[]);
void HUB75_WritePanel(uint8_t R, uint8_t G, uint8_t B, uint16_t row, uint16_t column);
void HUB75_LoadRGB565Frame(const uint16_t *frame, uint16_t width, uint16_t height);// load rgb565 frame to hub75 panel，if the frame is NULL，then clear the panel with black color
void HUB75_SetBrightness(uint8_t brightness);
void HUB75_SetRefreshRate(uint8_t level);

void DWT_Init(void);
void Delay_us(uint32_t us);
#endif
