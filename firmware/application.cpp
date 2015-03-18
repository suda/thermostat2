#include <math.h>
#include "application.h"
#include "Adafruit_mfGFX.h"
#include "Adafruit_ILI9341.h"
#include "scale.h"

// Comment out to use Honeywell HIH6131-021-001 I2C sensor
#define USE_DS18B20

#ifdef USE_DS18B20
  #include "DS18B20.h"
  #include "OneWire.h"

  #define ONE_WIRE_PIN  A7

  DS18B20 ds18b20 = DS18B20(ONE_WIRE_PIN);
  char szInfo[64];
#else
  #define TEMP_SENSOR 0x27
#endif
#define FAN_PIN     D0
#define HEAT_PIN    D7
#define POT_PIN     A6
#define PIR_PIN     D3
// #define USE_PIR     1
#define DESIRED_TEMP_FLASH_ADDRESS 0x80000
// #define USE_FAHRENHEIT

Adafruit_ILI9341 tft = Adafruit_ILI9341(A2, A1, A0);

static const uint8_t smile[] = {
  0b00111100,
  0b01000010,
  0b10100101,
  0b10000001,
  0b10100101,
  0b10011001,
  0b01000010,
  0b00111100
};

#define	COLOR_HEATING   0xF800
#define	COLOR_COOLING   0x001F

int currentTemperature = 0;
int desiredTemperature = 0;
bool isHeatOn = false;
bool isFanOn = false;
bool motionDetected = false;

int lastChangedPot = -80;
int i = 0;
void displayTemperature(void)
{
  if (isHeatOn) tft.fillScreen(COLOR_HEATING);
  else tft.fillScreen(COLOR_COOLING);

  tft.drawXBitmap(43, 13, scale_bits, scale_width, scale_height, ILI9341_WHITE);
  tft.setCursor(115, 95);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(8);
  // tft.println(desiredTemperature);
  tft.println(currentTemperature);
}

void saveTemperature()
{
  sFLASH_EraseSector(DESIRED_TEMP_FLASH_ADDRESS);
  Serial.println("Saving temperature to flash");
  uint8_t values[2] = { (uint8_t)desiredTemperature, 0 };
  sFLASH_WriteBuffer(values, DESIRED_TEMP_FLASH_ADDRESS, 2);
}

void loadTemperature()
{
  Serial.println("Loading and displaying temperature from flash");
  uint8_t values[2];
  sFLASH_ReadBuffer(values, DESIRED_TEMP_FLASH_ADDRESS, 2);
  desiredTemperature = values[0];
  displayTemperature();
}

int setTemperature(int t)
{
  desiredTemperature = t;
  displayTemperature();
  saveTemperature();
  return desiredTemperature;
}

int setTemperatureFromString(String t)
{
  // TODO more robust error handling
  //      what if t is not a number
  //      what if t is outside 50-90 range

  Serial.print("Setting desired temp from web to ");
  Serial.println(t);

  return setTemperature(t.toInt());
}

void initScreen()
{
  // TODO
  tft.begin();
  tft.setRotation(3);
}

void setup()
{
#ifdef USE_DS18B20

#else
  Wire.begin();
#endif

  initScreen();

  Spark.function("set_temp", setTemperatureFromString);

  Spark.variable("current_temp", &currentTemperature, INT);
  Spark.variable("desired_temp", &desiredTemperature, INT);
  Spark.variable("is_heat_on", &isHeatOn, BOOLEAN);
  Spark.variable("is_fan_on", &isFanOn, BOOLEAN);

  Serial.begin(9600);

  loadTemperature();
return;
  pinMode(FAN_PIN, OUTPUT);
  pinMode(HEAT_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);
#ifdef USE_PIR
  pinMode(PIR_PIN, INPUT);
#endif
}

void loop()
{
#ifdef USE_DS18B20
  if(!ds18b20.search()){
    ds18b20.resetsearch();
    delay(250);

    return;
  }
#endif

  static int wait = 0;
  if (!wait)
  {
    wait = 10;
    float fTemp;

#ifdef USE_DS18B20
    fTemp = ds18b20.getTemperature();
#else
    Wire.beginTransmission(TEMP_SENSOR);
    Wire.endTransmission();
    delay(40);
    Wire.requestFrom(TEMP_SENSOR, 4);
    uint8_t b = Wire.read();
    Serial.print("I2C Status bits are ");
    Serial.println(b >> 6);

    int humidity = (b << 8) & 0x3f00;
    humidity |= Wire.read();
    float percentHumidity = humidity / 163.83;
    Serial.print("Relative humidity is ");
    Serial.println(percentHumidity);

    int temp = (Wire.read() << 6) & 0x3fc0;
    temp |= Wire.read() >> 2;
    temp *= 165;
    fTemp = temp / 16383.0 - 40.0;
#endif

#ifdef USE_FAHRENHEIT
    fTemp = fTemp * 1.8 + 32.0; // convert to fahrenheit
#endif

    currentTemperature = roundf(fTemp);
    Serial.print("Temperature is ");
    Serial.println(fTemp);
    displayTemperature();
  }

  int pot = 4095 - analogRead(POT_PIN);
  if (1000 == wait)
  {
    Serial.print("Potentiometer reading: ");
    Serial.println(pot);

#ifdef USE_PIR
    Serial.print("PIR reading: ");
    Serial.println(analogRead(PIR_PIN));
#endif
  }

#ifdef USE_PIR
  if (3550 < analogRead(PIR_PIN))
  {
    motionDetected = true;
    // lean more toward comfort than energy efficiency
  }
#endif

  // If user has adjusted the potentiometer
  if (fabsf(pot - lastChangedPot) > 64)
  {
    // Don't set temp on boot
    if (lastChangedPot >= 0)
    {
      // map 0-4095 pot range to 50-90 temperature range
      int t = roundf(pot * (40.0/4095.0) + 50.0);
      setTemperature(t);
      Serial.print("Setting desired temp based on potentiometer to ");
      Serial.println(t);
    }
    lastChangedPot = pot;
  }

  isHeatOn = desiredTemperature > currentTemperature;
  digitalWrite(HEAT_PIN, isHeatOn);

  // just run them at the same time for now
  isFanOn = isHeatOn;
  digitalWrite(FAN_PIN, isFanOn);

  --wait;
}
