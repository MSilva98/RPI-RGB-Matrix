// testshapes demo for RGBmatrixPanel library.
// Demonstrates the drawing abilities of the RGBmatrixPanel library.
// For RGB LED matrix.

// WILL NOT FIT on ARDUINO UNO -- requires a Mega, M0 or M4 board

#include "RGBmatrixPanel.h"

#include "bit_bmp.h"
#include "Fonts/fonts.h"
#include <stdlib.h>
#include <string.h>


// HUB75 pin mapping (user-defined)
#define CONFIG_HUB75_PIN_R1 0
#define CONFIG_HUB75_PIN_G1 1
#define CONFIG_HUB75_PIN_B1 2
#define CONFIG_HUB75_PIN_R2 3
#define CONFIG_HUB75_PIN_G2 4
#define CONFIG_HUB75_PIN_B2 5
#define CONFIG_HUB75_PIN_A 7
#define CONFIG_HUB75_PIN_B 8
#define CONFIG_HUB75_PIN_C 9
#define CONFIG_HUB75_PIN_D 10
#define CONFIG_HUB75_PIN_E 6
#define CONFIG_HUB75_PIN_LAT 12
#define CONFIG_HUB75_PIN_OE 13
#define CONFIG_HUB75_PIN_CLK 11

#define CLK CONFIG_HUB75_PIN_CLK
#define OE CONFIG_HUB75_PIN_OE
#define LAT CONFIG_HUB75_PIN_LAT
#define A CONFIG_HUB75_PIN_A
#define B CONFIG_HUB75_PIN_B
#define C CONFIG_HUB75_PIN_C
#define D CONFIG_HUB75_PIN_D
#define E CONFIG_HUB75_PIN_E

#define Matrix_Width 96
#define Matrix_Height 48

uint8_t hub75_rgb_pins[] = { CONFIG_HUB75_PIN_R1, CONFIG_HUB75_PIN_G1,
                             CONFIG_HUB75_PIN_B1, CONFIG_HUB75_PIN_R2,
                             CONFIG_HUB75_PIN_G2, CONFIG_HUB75_PIN_B2 };
RGBmatrixPanel matrix(A, B, C, D, E, CLK, LAT, OE, false, Matrix_Width,
                      Matrix_Height / 2, hub75_rgb_pins);

static void panel_delay(uint32_t ms) {
  uint32_t start_ms = millis();
  while ((millis() - start_ms) < ms) {
    matrix.updateDisplay();
  }
}

// Configure the serial port to use the standard printf function

void setup() {
  Serial.begin(115200);
  matrix.begin();
  panel_delay(500);
}

void loop() {
  // Do nothing -- image doesn't change
  Demo_0();
  //  Demo_1();
  //  Demo_2();
}

// Clear screen
void screen_clear() {
  matrix.fillRect(0, 0, matrix.width(), matrix.height(),
                  matrix.Color333(0, 0, 0));
}

static int16_t centered_text_x(const char *text, int16_t y_baseline,
                               const GFXfont *font) {
  int16_t x1, y1;
  uint16_t w, h;
  matrix.setFont(font);
  matrix.setTextSize(1);
  matrix.setTextWrap(false);
  matrix.getTextBounds((char *)text, 0, y_baseline, &x1, &y1, &w, &h);
  int16_t x = (matrix.width() - (int16_t)w) / 2;
  if (x < 0) {
    x = 0;
  }
  return x;
}

static void print_centered_rainbow_text(int16_t y_baseline, const char *text,
                                        uint8_t color_offset) {
  matrix.setFont(NULL);
  matrix.setTextSize(1);
  matrix.setTextWrap(false);

  int16_t x = centered_text_x(text, y_baseline, NULL);
  matrix.setCursor(x, y_baseline);

  for (uint8_t i = 0; text[i] != 0; i++) {
    matrix.setTextColor(Wheel((i + color_offset) % 24));
    matrix.print(text[i]);
  }
}

void Demo_0() {
  int16_t panel_width = matrix.width();
  int16_t panel_height = matrix.height();
  int16_t min_size = (panel_width < panel_height) ? panel_width : panel_height;
  int16_t circle_radius = min_size / 6;
  if (circle_radius < 2) {
    circle_radius = 2;
  }

  // draw a pixel in solid white
  screen_clear();
  matrix.setFont(NULL);
  matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));
  panel_delay(500);

  matrix.fillRect(0, 0, matrix.width(), matrix.height(),
                  matrix.Color333(0, 7, 0));
  panel_delay(500);

  matrix.drawRect(0, 0, matrix.width(), matrix.height(),
                  matrix.Color333(7, 7, 0));
  panel_delay(500);

  // draw an 'X' in red
  matrix.drawLine(0, 0, matrix.width() - 1, matrix.height() - 1,
                  matrix.Color333(7, 0, 0));
  matrix.drawLine(matrix.width() - 1, 0, 0, matrix.height() - 1,
                  matrix.Color333(7, 0, 0));
  panel_delay(500);

  // draw a blue circle
  matrix.drawCircle(circle_radius + 1, circle_radius + 1, circle_radius,
                    matrix.Color333(0, 0, 7));
  panel_delay(500);

  // fill a violet circle
  matrix.fillCircle((panel_width * 3) / 4, panel_height / 3, circle_radius,
                    matrix.Color333(7, 0, 7));
  panel_delay(500);

  // fill the screen with 'black'
  screen_clear();

  // draw some text!
  matrix.setTextSize(1);      // size 1 == 8 pixels high
  matrix.setTextWrap(false);  // Don't wrap at end of line - will do ourselves

  uint8_t draw_y = Matrix_Height / 4;
  print_centered_rainbow_text(draw_y * 0, "Waveshare", 0);
  print_centered_rainbow_text(draw_y * 1, "Electronics", 4);
  print_centered_rainbow_text(draw_y * 2, "RGB MATRIX", 8);
  char resolution_text[24];
  snprintf(resolution_text, sizeof(resolution_text), "%dx%d  RGB",
          panel_width, panel_height);
  print_centered_rainbow_text(draw_y * 3, resolution_text, 12);

  panel_delay(2000);
}

// Input a value 0 to 7 to get a color value.
// The colours are a transition r - g - b - back to r.
uint16_t Wheel(byte WheelPos) {
  if (WheelPos < 8) {
    return matrix.Color333(7 - WheelPos, WheelPos, 0);
  } else if (WheelPos < 16) {
    WheelPos -= 8;
    return matrix.Color333(0, 7 - WheelPos, WheelPos);
  } else {
    WheelPos -= 16;
    return matrix.Color333(WheelPos, 0, 7 - WheelPos);
  }
}

/*  @name :  display_Image
 *  @brief:  display an image
 *           The image data is in the "bit_bmp.h"
 *           You can use some tools to get the image data
 *           You can set the data bits which is 8 or 16 and set the MSB first
 * which is true or false on line 1001 of Adafruit_GFX.c
 *  @param:    x   Top left corner x coordinate
 *             y   Top left corner y coordinate
 *         bitmap  byte array with 16-bit color bitmap,the image data is in the
 * "bit_bmp.h" w   Width of bitmap in pixels h   Height of bitmap in pixels
 *  @retval: None
 */
void display_Image(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w,
                   int16_t h) {
  matrix.display_image(x, y, bitmap, w, h);
}

#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSerif9pt7b.h"
#include "Fonts/FreeSerifBoldItalic9pt7b.h"
#include "Fonts/RobotoMono_Thin7pt7b.h"


/*  @name : display_text
 *  @brief: display a text string
 *  @param:    x            X coordinate in pixels
 *             y            Y coordinate in pixels
 *           *str           Text string
 *            *f            Text font,if you use the default font,the value will
 * be NULL, if you use other font ,you should add header file of font to folder
 * named “Fonts” like this Demo color          16-bit 5-6-5 Color to draw text
 * with pixels_size      Desired text size. 1 is default 6x8, 2 is 12x16, 3 is
 * 18x24, etc
 *
 *  @retval: None
 */
void display_text(int x, int y, char *str, const GFXfont *f, int color,
                  int pixels_size) {
  matrix.setTextSize(pixels_size);  // size 1 == 8 pixels high
  matrix.setTextWrap(false);        // Don't wrap at end of line - will do ourselves
  matrix.setFont(f);                // set font
  matrix.setCursor(x, y);
  matrix.setTextColor(color);
  matrix.println(str);
}

void Demo_1() {
  screen_clear();
  display_text(-1, 14, "R", &FreeSerif9pt7b, matrix.Color333(7, 0, 0), 1);
  display_text(8, 14, "G", &FreeSerif9pt7b, matrix.Color333(0, 7, 0), 1);
  display_text(19, 14, "B", &FreeSerif9pt7b, matrix.Color333(0, 0, 7), 1);
  display_text(31, 5, "m", NULL, 0x7800, 1);
  display_text(37, 5, "a", NULL, 0xFFE0, 1);
  display_text(42, 5, "t", NULL, 0x07E0, 1);
  display_text(48, 5, "r", NULL, 0X001F, 1);
  display_text(53, 5, "i", NULL, 0x07FF, 1);
  display_text(58, 5, "x", NULL, 0x780F, 1);
  display_text(-1, 30, "P2", &RobotoMono_Thin7pt7b, 0xFFE0, 1);
  display_text(12, 25, ".", NULL, 0xFFE0, 1);
  display_text(21, 30, "6", &FreeSerifBoldItalic9pt7b, 0x780F, 1);
  display_text(29, 30, "4", &FreeSerifBoldItalic9pt7b, 0x780F, 1);
  display_text(38, 30, "x", &FreeSans9pt7b, 0x780F, 1);
  display_text(46, 30, "6", &FreeSerifBoldItalic9pt7b, 0x780F, 1);
  display_text(54, 30, "4", &FreeSerifBoldItalic9pt7b, 0x780F, 1);

  display_text(-1, 14 + 32, "R", &FreeSerif9pt7b, matrix.Color333(7, 0, 0), 1);
  display_text(8, 14 + 32, "G", &FreeSerif9pt7b, matrix.Color333(0, 7, 0), 1);
  display_text(19, 14 + 32, "B", &FreeSerif9pt7b, matrix.Color333(0, 0, 7), 1);
  display_text(31, 5 + 32, "M", NULL, 0x7800, 1);
  display_text(37, 5 + 32, "a", NULL, 0xFFE0, 1);
  display_text(42, 5 + 32, "t", NULL, 0x07E0, 1);
  display_text(48, 5 + 32, "r", NULL, 0X001F, 1);
  display_text(53, 5 + 32, "i", NULL, 0x07FF, 1);
  display_text(58, 5 + 32, "x", NULL, 0x780F, 1);
  display_text(-1, 30 + 32, "P2", &RobotoMono_Thin7pt7b, 0xFFE0, 1);
  display_text(12, 25 + 32, ".", NULL, 0xFFE0, 1);
  display_text(21, 30 + 32, "6", &FreeSerifBoldItalic9pt7b, 0x780F, 1);
  display_text(29, 30 + 32, "4", &FreeSerifBoldItalic9pt7b, 0x780F, 1);
  display_text(38, 30 + 32, "x", &FreeSans9pt7b, 0x780F, 1);
  display_text(46, 30 + 32, "6", &FreeSerifBoldItalic9pt7b, 0x780F, 1);
  display_text(54, 30 + 32, "4", &FreeSerifBoldItalic9pt7b, 0x780F, 1);
  panel_delay(2000);

  display_Image(0, 0, gImage_image, 64, 64);
  panel_delay(2000);
}

void Demo_2() {
  screen_clear();
  matrix.DrawString_CN(1, 12, "微", &Font16CN, matrix.Color333(7, 0, 0));
  matrix.DrawString_CN(1 + 16, 12, "雪", &Font16CN, matrix.Color333(6, 1, 0));
  matrix.DrawString_CN(1 + 32, 12, "电", &Font16CN, matrix.Color333(5, 2, 7));
  matrix.DrawString_CN(1 + 48, 12, "子", &Font16CN, matrix.Color333(0, 7, 0));
  matrix.DrawString_CN(1, 35, "欢", &Font16CN, matrix.Color333(0, 6, 1));
  matrix.DrawString_CN(1 + 16, 35, "迎", &Font16CN, matrix.Color333(0, 5, 2));
  matrix.DrawString_CN(1 + 32, 35, "您", &Font16CN, matrix.Color333(0, 0, 7));
  matrix.DrawString_CN(1 + 48, 35, "！", &Font16CN, matrix.Color333(1, 0, 6));
  panel_delay(2000);
  screen_clear();
  matrix.DrawString_CN(1, 0, "微", &Font32CN, matrix.Color333(0, 7, 7));
  matrix.DrawString_CN(1 + 32, 0, "雪", &Font32CN, matrix.Color333(0, 7, 7));
  matrix.DrawString_CN(1, 32, "电", &Font32CN, matrix.Color333(0, 7, 7));
  matrix.DrawString_CN(1 + 32, 32, "子", &Font32CN, matrix.Color333(0, 7, 7));
  panel_delay(2000);
  screen_clear();
  matrix.DrawString_CN(1, 0, "欢", &Font32CN, matrix.Color333(0, 7, 7));
  matrix.DrawString_CN(1 + 32, 0, "迎", &Font32CN, matrix.Color333(0, 7, 7));
  matrix.DrawString_CN(1, 32, "您", &Font32CN, matrix.Color333(0, 7, 7));
  matrix.DrawString_CN(1 + 37, 32, "！", &Font32CN, matrix.Color333(0, 7, 7));
  panel_delay(2000);
  screen_clear();
  matrix.DrawString_CN(0, 0, "微", &Font64CN, matrix.Color333(7, 0, 0));
  panel_delay(1000);
  screen_clear();
  matrix.DrawString_CN(0, 0, "雪", &Font64CN, matrix.Color333(0, 7, 0));
  panel_delay(1000);
  screen_clear();
  matrix.DrawString_CN(0, 0, "电", &Font64CN, matrix.Color333(0, 0, 7));
  panel_delay(1000);
  screen_clear();
  matrix.DrawString_CN(0, 0, "子", &Font64CN, matrix.Color333(7, 7, 7));
  panel_delay(1000);
  screen_clear();
  matrix.DrawString_CN(0, 0, "欢", &Font64CN, matrix.Color333(7, 5, 0));
  panel_delay(1000);
  screen_clear();
  matrix.DrawString_CN(0, 0, "迎", &Font64CN, matrix.Color333(0, 7, 5));
  panel_delay(1000);
  screen_clear();
  matrix.DrawString_CN(0, 0, "您", &Font64CN, matrix.Color333(5, 0, 7));
  panel_delay(1000);
  screen_clear();
  matrix.DrawString_CN(13, 0, "！", &Font64CN, matrix.Color333(7, 7, 7));
  panel_delay(2000);
  screen_clear();
}
