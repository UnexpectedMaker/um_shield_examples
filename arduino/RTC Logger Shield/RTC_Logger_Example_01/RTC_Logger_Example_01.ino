/*
 * Unexpected Maker RTC Logger Shield Example
 * 
 * Wake from Deep Sleep once per hour, and log some data to a file on the SD Card 
 * Use the RTC to set the exact time every hour to wake up
 * 
 * Compatible with:
 * TinyPICO https://unexpectedmaker.com/TinyPICO
 * TinyS2   https://unexpectedmaker.com/TinyS2
 * TinyS3   https://unexpectedmaker.com/TinyS3
 * TinyC6   https://unexpectedmaker.com/TinyC6
 * 
 * 2024 Unexpected Maker
 */

#include <RV3028C7.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#if defined(ARDUINO_TINYC6)

#define PIN_RTC_INT       3
#define PIN_RTC_CLK_OUT   2
#define PIN_SD_DETECT     1
#define PIN_SD_CS         18
#define PIN_I2C_SDA2      11
#define PIN_I2C_SCL2      0
#define PIN_WAKE_IO       GPIO_NUM_3

#elif defined(ARDUINO_TINYS3)

#define PIN_RTC_INT       1
#define PIN_RTC_CLK_OUT   2
#define PIN_SD_DETECT     3
#define PIN_SD_CS         34
#define PIN_I2C_SDA2      5
#define PIN_I2C_SCL2      4
#define PIN_WAKE_IO       GPIO_NUM_1

#elif defined(ARDUINO_TINYS2)

#define PIN_RTC_INT       4
#define PIN_RTC_CLK_OUT   5
#define PIN_SD_DETECT     6
#define PIN_SD_CS         14
#define PIN_I2C_SDA2      11
#define PIN_I2C_SCL2      0
#define PIN_WAKE_IO       GPIO_NUM_4

#else

#define PIN_RTC_INT       4
#define PIN_RTC_CLK_OUT   14
#define PIN_SD_DETECT     15
#define PIN_SD_CS         5
#define PIN_I2C_SDA2      26
#define PIN_I2C_SCL2      27
#define PIN_WAKE_IO       GPIO_NUM_4

#endif

// Create an RTC class object
RV3028C7 rtc;

bool rtc_found = true;
bool sdcard_found = true;
bool read_and_log_data = false;

const char * data_filename = "/data_log.txt";

esp_sleep_wakeup_cause_t wakeup_reason;

void setup()
{
  // Grab the wakeup reason for why the code is starting
  wakeup_reason = esp_sleep_get_wakeup_cause();

  // I2S uses the defaul I2C pins detected for your TinyX board based on defined pins in "arduino.h"
  Wire.begin();

  Serial.begin(115200);
  // Serial output is delayed with native USB, but we don't want a while loop
  // as then no Serial will block battery use
  delay(1000);

  Serial.println();
  Serial.println("RTC Logger Example");

  /* Setup the RTC */

  // Initialise and start up the RTC. If init fails, we flag no RTC available
  if (rtc.begin() == false)
  {
    Serial.println("Failed to detect RV-3028-C7!\nCheck your wiring or soldering...");
    rtc_found = false;
  }
  else
  {
    Serial.println("Found RV-3028-C7!");

    // Did we wake from Deep Sleep or not?
    if (wakeup_reason != ESP_SLEEP_WAKEUP_EXT0)
    {
      // No we didn't wake from sleep, so let's set the date and time.
      // This should be via WiFi and NTP, but for now we'll hard code it.
      rtc.setDateTimeFromISO8601("2024-04-09T08:00:00");
      rtc.synchronize(); // Writes the new date time to RTC

      // Print the current date and time to the serial output
      Serial.print("RTC Set Initial Date/Time: ");
      Serial.println(rtc.getCurrentDateTime());

      // Because this is the initial boot and not a wake from sleep,
      // do we want to log data at this point or wate for the RTC alarm to go off?
      // Comment the next line to prevent the data from being logged on bootup
      read_and_log_data = true;
    }
    else
    {
      // Yes, we did wake from deep sleep, so print the current time
      // and then set the interrupt trigger to true
      Serial.print("RTC Wake Date/Time: ");
      Serial.println(rtc.getCurrentDateTime());

      read_and_log_data = true;
    }

    // Now we set the RTC alarm to fire once every hour when the time matches xx:10 mins.
    // Example: 3:10, 10:10, 12:10 etc
    rtc.setHourlyAlarm(/*minute=*/01);
    rtc.enableInterrupt(INTERRUPT_ALARM);

    // And we tell the ESP32 to wake up when the GPIO_NUM of the RTC INYT pin goes low (Active Low)
    esp_sleep_enable_ext0_wakeup(PIN_WAKE_IO, 0); //1 = High, 0 = Low
  }

  Serial.println();

  /* Setup the SD Card */

  // We use this pin to detect if an SD card is inserted or not
  // This pin can also be disconnected from the SD Detect connection
  // via a jumper on the back of the shield, if you need the IO
  // for something else
  pinMode(PIN_SD_DETECT, INPUT);

  sdcard_found = digitalRead(PIN_SD_DETECT);

  // Initialise the SD card subsystem and look for a card
  if (!SD.begin(PIN_SD_CS) || !sdcard_found)
  {
    Serial.println("Card Mount Failed or no card found!");
    sdcard_found = false;
  }
  else
  {
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
      Serial.println("No SD Card attached");
      sdcard_found = false;
    }

    sdcard_found = true;

    Serial.print("SD Card Found - Type: ");
    if (cardType == CARD_MMC)
      Serial.println("MMC");
    else if (cardType == CARD_SD)
      Serial.println("SDSC");
    else if (cardType == CARD_SDHC)
      Serial.println("SDHC");
    else
      Serial.println("UNKNOWN");

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    Serial.println();
  }
}

void loop()
{
  // If we dont have an SD Card or the RTC has not initialised, we do nothing here.
  if (!rtc_found || !sdcard_found)
    return;

  if (read_and_log_data)
  {
    // Capture some data here, maybe from a sensor etc
    // and then log it to the SD Card
    // For now we fake a temperature reading

    String temperature = String(random(0, 50)) + "C";
    String data_to_log = String(rtc.getCurrentDateTime()) + "," + temperature + "\n";

    // Now append this data to the existing log on the SD card
    fileAppend(SD, data_filename, data_to_log.c_str());

    Serial.println("Going to sleep now");
    esp_deep_sleep_start();
  }
}

bool fileAppend(fs::FS & fs, const char * filename, const char * _data)
{
  Serial.printf("Appending to file: %s\n", filename);
  Serial.printf("With data: %s\n", _data);

  File file = fs.open(filename, FILE_APPEND);
  if (!file)
  {
    Serial.println("Failed to open file for appending");
    return false;
  }
  bool success = false;
  if (file.print(_data))
    success = true;

  file.close();
  return success;
}
