/*
   Unexpected Maker IPS 1.3" LCD Display Shield example
   https://unexpectedmaker.com/ips_13_display_shield

   Adafruit ST7789 Driver Graphics Test Example
   Jan 7, 2025
*/

#include <Adafruit_GFX.h>	 // Grab from library Manager
#include <Adafruit_ST7789.h> // Grab from library Manager
#include <SPI.h>

#if defined(ARDUINO_TINYS3)

#define AMB_LIGHT 1
#define TFT_BACKLIGHT 2
#define TFT_CS 34
#define TFT_RST 6
#define TFT_DC 7

#elif defined(ARDUINO_TINYS2)

#define AMB_LIGHT 4
#define TFT_BACKLIGHT 5
#define TFT_CS 14
#define TFT_RST 33
#define TFT_DC 38

#elif defined(ARDUINO_TINYC6)

#define AMB_LIGHT 3
#define TFT_BACKLIGHT 2
#define TFT_CS 18
#define TFT_RST 15
#define TFT_DC 5

#else

#define AMB_LIGHT 4
#define TFT_BACKLIGHT 14
#define TFT_CS 5
#define TFT_RST 33
#define TFT_DC 32

#endif

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup(void)
{
	Serial.begin(115200);

	// Set the ADC bit resolution for the AMB Light Sensor
	analogReadResolution(12);

#if defined(ARDUINO_TINYS3) || defined(ARDUINO_TINYS2)
	// Add delay to allow USB to start up
	delay(2000);
#endif

	Serial.print("ST7789 Display Example");

	tft.init(240, 240);

	tft.setSPISpeed(40000000);
	Serial.println("Initialized");

	tft.fillScreen(ST77XX_BLACK);

	// tft print function!
	tftPrintTest();
	delay(5000);
}

void loop()
{
	// Read the Ambient Light Sensor value and adjust the display
	// backlight brightness accordingly.
	// Remapping the AMB input to a PWM range of 100-255
	// This will only update once every loop through the graphics demo
	int ambValue = analogRead(AMB_LIGHT);
	int backLight = constrain(map(ambValue, 0, 4096, 100, 255), 0, 255);
	Serial.printf("ambVal: %d, backLight: %d\n", ambValue, backLight);
	analogWrite(TFT_BACKLIGHT, backLight);

	// line draw test
	testlines(ST77XX_YELLOW);
	delay(500);

	// optimized lines
	testfastlines(ST77XX_RED, ST77XX_BLUE);
	delay(500);

	testdrawrects(ST77XX_GREEN);
	delay(500);

	testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
	delay(500);

	tft.fillScreen(ST77XX_BLACK);
	testfillcircles(10, ST77XX_BLUE);
	testdrawcircles(10, ST77XX_WHITE);
	delay(500);

	testroundrects();
	delay(500);

	testtriangles();
	delay(500);
}

void testlines(uint16_t color)
{
	tft.fillScreen(ST77XX_BLACK);
	for (int16_t x = 0; x < tft.width(); x += 6)
	{
		tft.drawLine(0, 0, x, tft.height() - 1, color);
		delay(0);
	}
	for (int16_t y = 0; y < tft.height(); y += 6)
	{
		tft.drawLine(0, 0, tft.width() - 1, y, color);
		delay(0);
	}

	tft.fillScreen(ST77XX_BLACK);
	for (int16_t x = 0; x < tft.width(); x += 6)
	{
		tft.drawLine(tft.width() - 1, 0, x, tft.height() - 1, color);
		delay(0);
	}
	for (int16_t y = 0; y < tft.height(); y += 6)
	{
		tft.drawLine(tft.width() - 1, 0, 0, y, color);
		delay(0);
	}

	tft.fillScreen(ST77XX_BLACK);
	for (int16_t x = 0; x < tft.width(); x += 6)
	{
		tft.drawLine(0, tft.height() - 1, x, 0, color);
		delay(0);
	}
	for (int16_t y = 0; y < tft.height(); y += 6)
	{
		tft.drawLine(0, tft.height() - 1, tft.width() - 1, y, color);
		delay(0);
	}

	tft.fillScreen(ST77XX_BLACK);
	for (int16_t x = 0; x < tft.width(); x += 6)
	{
		tft.drawLine(tft.width() - 1, tft.height() - 1, x, 0, color);
		delay(0);
	}
	for (int16_t y = 0; y < tft.height(); y += 6)
	{
		tft.drawLine(tft.width() - 1, tft.height() - 1, 0, y, color);
		delay(0);
	}
}

void testdrawtext(char *text, uint16_t color)
{
	tft.setCursor(0, 0);
	tft.setTextColor(color);
	tft.setTextWrap(true);
	tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2)
{
	tft.fillScreen(ST77XX_BLACK);
	for (int16_t y = 0; y < tft.height(); y += 5)
	{
		tft.drawFastHLine(0, y, tft.width(), color1);
	}
	for (int16_t x = 0; x < tft.width(); x += 5)
	{
		tft.drawFastVLine(x, 0, tft.height(), color2);
	}
}

void testdrawrects(uint16_t color)
{
	tft.fillScreen(ST77XX_BLACK);
	for (int16_t x = 0; x < tft.width(); x += 6)
	{
		tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color);
	}
}

void testfillrects(uint16_t color1, uint16_t color2)
{
	tft.fillScreen(ST77XX_BLACK);
	for (int16_t x = tft.width() - 1; x > 6; x -= 6)
	{
		tft.fillRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color1);
		tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color2);
	}
}

void testfillcircles(uint8_t radius, uint16_t color)
{
	for (int16_t x = radius; x < tft.width(); x += radius * 2)
	{
		for (int16_t y = radius; y < tft.height(); y += radius * 2)
		{
			tft.fillCircle(x, y, radius, color);
		}
	}
}

void testdrawcircles(uint8_t radius, uint16_t color)
{
	for (int16_t x = 0; x < tft.width() + radius; x += radius * 2)
	{
		for (int16_t y = 0; y < tft.height() + radius; y += radius * 2)
		{
			tft.drawCircle(x, y, radius, color);
		}
	}
}

void testtriangles()
{
	tft.fillScreen(ST77XX_BLACK);
	uint16_t color = 0xF800;
	int t;
	int w = tft.width() / 2;
	int x = tft.height() - 1;
	int y = 0;
	int z = tft.width();
	for (t = 0; t <= 15; t++)
	{
		tft.drawTriangle(w, y, y, x, z, x, color);
		x -= 4;
		y += 4;
		z -= 4;
		color += 100;
	}
}

void testroundrects()
{
	tft.fillScreen(ST77XX_BLACK);
	uint16_t color = 100;
	int i;
	int t;
	for (t = 0; t <= 4; t += 1)
	{
		int x = 0;
		int y = 0;
		int w = tft.width() - 2;
		int h = tft.height() - 2;
		for (i = 0; i <= 16; i += 1)
		{
			tft.drawRoundRect(x, y, w, h, 5, color);
			x += 2;
			y += 3;
			w -= 4;
			h -= 6;
			color += 1100;
		}
		color += 100;
	}
}

void tftPrintTest()
{
	tft.setTextWrap(false);
	tft.fillScreen(ST77XX_BLACK);
	tft.setCursor(0, 30);
	tft.setTextColor(ST77XX_BLUE);
	tft.setTextSize(2);
	tft.println("Unexpected Maker\n");
	tft.setTextColor(ST77XX_GREEN);
	tft.setTextSize(3);
	tft.println("IPS 1.3\" LCD\n");
	tft.setTextColor(ST77XX_RED);
	tft.setTextSize(4);
	tft.println("HAZAH!\n\n");
	tft.setTextColor(ST77XX_YELLOW);
	tft.setTextSize(3);
	tft.println("DEMO TIME");
}
