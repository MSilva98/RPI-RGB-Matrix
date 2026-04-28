
// Example sketch which shows how to display some patterns
// on a 64x32 LED matrix
//

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>


#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another
 
//MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK, myWHITE, myRED, myGREEN, myBLUE;

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// From: https://gist.github.com/davidegironi/3144efdc6d67e5df55438cc3cba613c8
uint16_t colorWheel(uint8_t pos) {
  if(pos < 85) {
    return dma_display->color565(pos * 3, 255 - pos * 3, 0);
  } else if(pos < 170) {
    pos -= 85;
    return dma_display->color565(255 - pos * 3, 0, pos * 3);
  } else {  
    pos -= 170;
    return dma_display->color565(0, pos * 3, 255 - pos * 3);
  }
}

void drawText(int colorWheelOffset)
{
  
  // draw text with a rotating colour
  const char *line1 = "ESP32 DMA";
  const char *line2 = "*********";
  const char *line3 = "LED MATRIX!";
  const char *line4 = "64x32 *RGB*";

  const int16_t display_w = dma_display->width();
  const int16_t display_h = dma_display->height();

  dma_display->setTextWrap(false);

  int8_t text_size = 1;
  for (int8_t candidate_size = 1; candidate_size <= 4; candidate_size++) {
    dma_display->setTextSize(candidate_size);


    int16_t x1 = 0, y1 = 0;
    uint16_t w1 = 0, h1 = 0;

    dma_display->getTextBounds(line1, 0, 0, &x1, &y1, &w1, &h1);
    const bool fits_1 = (int16_t)w1 <= display_w;

    dma_display->getTextBounds(line2, 0, 0, &x1, &y1, &w1, &h1);
    const bool fits_2 = (int16_t)w1 <= display_w;

    dma_display->getTextBounds(line3, 0, 0, &x1, &y1, &w1, &h1);
    const bool fits_3 = (int16_t)w1 <= display_w;

    dma_display->getTextBounds(line4, 0, 0, &x1, &y1, &w1, &h1);
    const bool fits_4 = (int16_t)w1 <= display_w;

    const int16_t line_h = 8 * candidate_size;
    const bool fits_v = (4 * line_h) <= display_h;

    if (fits_1 && fits_2 && fits_3 && fits_4 && fits_v) {
      text_size = candidate_size;
    }
  }

  dma_display->setTextSize(text_size);

  const int16_t line_h = 8 * text_size;
  int16_t remaining_h = display_h - (4 * line_h);
  if (remaining_h < 0) {
    remaining_h = 0;
  }
  const int16_t gap_h = remaining_h / 5;

  int16_t x1 = 0, y1 = 0;
  uint16_t tw = 0, th = 0;

  dma_display->getTextBounds(line1, 0, 0, &x1, &y1, &tw, &th);
  int16_t x_line1 = (display_w - (int16_t)tw) / 2;
  if (x_line1 < 0) x_line1 = 0;
  const int16_t y_line1 = gap_h + (0 * (line_h + gap_h));
  dma_display->setCursor(x_line1, y_line1);
  for (uint16_t i = 0; i < strlen(line1); i++) {
    dma_display->setTextColor(colorWheel((i * 32) + colorWheelOffset));
    dma_display->print(line1[i]);
  }

  dma_display->getTextBounds(line2, 0, 0, &x1, &y1, &tw, &th);
  int16_t x_line2 = (display_w - (int16_t)tw) / 2;
  if (x_line2 < 0) x_line2 = 0;
  const int16_t y_line2 = gap_h + (1 * (line_h + gap_h));
  dma_display->setCursor(x_line2, y_line2);
  for (uint16_t i = 0; i < strlen(line2); i++) {
    dma_display->setTextColor(colorWheel(((i + 9) * 32) + colorWheelOffset));
    dma_display->print(line2[i]);
  }

  dma_display->getTextBounds(line3, 0, 0, &x1, &y1, &tw, &th);
  int16_t x_line3 = (display_w - (int16_t)tw) / 2;
  if (x_line3 < 0) x_line3 = 0;
  const int16_t y_line3 = gap_h + (2 * (line_h + gap_h));
  dma_display->setCursor(x_line3, y_line3);
  dma_display->setTextColor(dma_display->color444(15, 15, 15));
  dma_display->print(line3);

  dma_display->getTextBounds(line4, 0, 0, &x1, &y1, &tw, &th);
  int16_t x_line4 = (display_w - (int16_t)tw) / 2;
  if (x_line4 < 0) x_line4 = 0;
  const int16_t y_line4 = gap_h + (3 * (line_h + gap_h));
  dma_display->setCursor(x_line4, y_line4);
  for (uint16_t i = 0; i < strlen(line4); i++) {
    const char c = line4[i];
    if (c == '*') {
      if (i < strlen(line4) - 1 && line4[i + 1] == 'R') dma_display->setTextColor(dma_display->color444(0, 15, 15));
      else dma_display->setTextColor(dma_display->color444(15, 0, 8));
    }
    else if (c == 'R') dma_display->setTextColor(dma_display->color444(15, 0, 0));
    else if (c == 'G') dma_display->setTextColor(dma_display->color444(0, 15, 0));
    else if (c == 'B') dma_display->setTextColor(dma_display->color444(0, 0, 15));
    else if (c == ' ') dma_display->setTextColor(dma_display->color444(15, 15, 15));
    else dma_display->setTextColor(colorWheel((i * 32) + colorWheelOffset));

    dma_display->print(c);
  }

}


void setup() {

  // Module configuration
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
  );

  mxconfig.gpio.e = 9;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6126A;
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_20M;   
  mxconfig.min_refresh_rate = 240; 
  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90); //0-255
  dma_display->clearScreen();

  myBLACK = dma_display->color565(0, 0, 0);
  myWHITE = dma_display->color565(255, 255, 255);
  myRED = dma_display->color565(255, 0, 0);
  myGREEN = dma_display->color565(0, 255, 0);
  myBLUE = dma_display->color565(0, 0, 255);
  

  dma_display->fillScreen(myWHITE);
  
  // fix the screen with green
  dma_display->fillRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(0, 15, 0));
  delay(500);

  // draw a box in yellow
  dma_display->drawRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(15, 15, 0));
  delay(500);

  // draw an 'X' in red
  dma_display->drawLine(0, 0, dma_display->width()-1, dma_display->height()-1, dma_display->color444(15, 0, 0));
  dma_display->drawLine(dma_display->width()-1, 0, 0, dma_display->height()-1, dma_display->color444(15, 0, 0));
  delay(500);

  // draw a blue circle
  dma_display->drawCircle(10, 10, 10, dma_display->color444(0, 0, 15));
  delay(500);

  // fill a violet circle
  dma_display->fillCircle(40, 21, 10, dma_display->color444(15, 0, 15));
  delay(500);

  // fill the screen with 'black'
  dma_display->fillScreen(dma_display->color444(0, 0, 0));

  //drawText(0);

}

uint8_t wheelval = 0;
void loop() {

  // animate by going through the colour wheel for the first two lines
  drawText(wheelval);
  wheelval +=1;
  delay(2000);
  dma_display->clearScreen();
  dma_display->fillScreen(myBLACK);
  delay(2000);
  dma_display->fillScreen(myRED);
  delay(2000);
  dma_display->fillScreen(myGREEN);
  delay(2000);
  dma_display->fillScreen(myBLUE);
  delay(2000);
  dma_display->fillScreen(myWHITE);
  delay(2000);
  dma_display->clearScreen();

}
