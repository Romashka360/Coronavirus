#include <Adafruit_GFX.h> 
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Wire.h>
#include "MLX90640_API.h" //github.com/sparkfun/SparkFun_MLX90640_Arduino_Example
#include "MLX90640_I2C_Driver.h"

#define TA_SHIFT 8 //Default shift for MLX90640 in open air
#define RGB565(r,g,b) (((b&0xF8)<<8) | ((g&0xFC)<<3) | ((r&0xF8)>>3))
Adafruit_ST7735 tft = Adafruit_ST7735(/*TFT_CS*/ 11, /*TFT_DC*/ 10, /*TFT_MOSI*/ 13, /*TFT_SCLK*/ 12, /*TFT_RST*/ 9);

const byte MLX90640_address = 0x33;
static float mlx90640To[768];
paramsMLX90640 mlx90640;


void setup()
{
  Wire.begin();
  Wire.setClock(400000); //Increase I2C clock speed to 400kHz
  Wire.beginTransmission((uint8_t)MLX90640_address);
  uint16_t eeMLX90640[832];
  MLX90640_DumpEE(MLX90640_address, eeMLX90640);
  MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
  MLX90640_SetRefreshRate(MLX90640_address, 0x02); //Set rate to 2Hz

  tft.initR(INITR_MINI160x80);
  tft.fillScreen(ST77XX_WHITE);
  tft.setRotation(3);
  tft.invertDisplay(true);
}


void loop()
{
String myT;
uint16_t mlx90640Frame[834];

    MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
    float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
    float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
    float tr = Ta - TA_SHIFT;
    float emissivity = 0.95;
    MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);

 for (int i1 = 0 ; i1 < 32 ; i1++)  {
   for (int i2 = 0 ; i2 < 24 ; i2++)  {
    float tT = mlx90640To[i1+i2*32];
    float mycolor;

    //get Iron Palette 
    mycolor = 0;//Black
    mycolor = (tT>=30?RGB565(0x20, 0x00, 0x8C):mycolor);//Blue
    mycolor = (tT>=80?RGB565(0xCC, 0x00, 0x77):mycolor);//Purple
    mycolor = (tT>=130?RGB565(0xFF, 0xD7, 0x00):mycolor);//Gold
    mycolor = (tT>=180?RGB565(0xFF, 0xFF, 0xFF):mycolor);//White

    tft.fillRect(1+i1*3, tft.height()-i2*3, 3,3, mycolor);
    if (i1+i2*32==768/2+15) {myT=String(tT,0); tft.fillRect(1+i1*3, tft.height()-i2*3, 3,3, RGB565(0x00, 0x00, 0xff));} //Center
  }
 }

  tft.fillRect(100,1, 60,60, ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_BLACK);
  tft.setCursor(100, 30);
  tft.print(myT+"C");
} 
