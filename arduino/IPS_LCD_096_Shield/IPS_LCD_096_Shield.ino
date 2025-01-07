/*
 * Unexpected Maker IPS LCD Display shield example
 * https://unexpectedmaker.com/ips_display_shield
 * 
 * Adafruit ST7789 Driver Graphics Test Example
 * Jan 30, 2024
 *
 */

#include <Adafruit_GFX.h>	 // Grab from library Manager
#include <Adafruit_ST7735.h> // Grab from library Manager
#include <SPI.h>

#if defined(ARDUINO_TINYS3)

#define TFT_CS 34
#define TFT_RST -1
#define TFT_DC 3
#define TFT_BACKLIGHT 2

#elif defined(ARDUINO_TINYS2)

#define TFT_CS 14
#define TFT_RST -1
#define TFT_DC 6
#define TFT_BACKLIGHT 5

#else

#define TFT_CS 5
#define TFT_RST -1
#define TFT_DC 15
#define TFT_BACKLIGHT 14

#endif

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup(void)
{
	tft.initR(INITR_MINI160x80);
	tft.invertDisplay(false);

	for (int i = 0; i < 2; i++)
	{
		tft.fillScreen(ST77XX_RED);

		delay(200);

		tft.fillScreen(ST77XX_GREEN);

		delay(200);

		tft.fillScreen(ST77XX_BLUE);

		delay(200);
	}

	tft.setTextSize(3);

	tft.invertDisplay(true); // Where i is true or false

	tft.fillScreen(ST77XX_BLACK);

	tft.setCursor(0, 0);

	tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
	tft.println("Invert\nON\n");

	delay(400);

	tft.invertDisplay(false); // Where i is true or false

	tft.fillScreen(ST77XX_BLACK);

	tft.setCursor(0, 0);

	tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
	tft.println("Invert\nOFF\n");

	delay(400);
}

void loop()
{
}
