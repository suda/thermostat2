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
#define ENCODER_PIN_A D0
#define ENCODER_PIN_B D1
#define PIR_PIN     D3
// #define USE_PIR     1
#define DESIRED_TEMP_FLASH_ADDRESS 0x80000
// #define USE_FAHRENHEIT
#define	COLOR_HEATING   0xF800
#define	COLOR_COOLING   0x001F
#define TEMP_MIN  5
#define TEMP_MAX  40
#define TEMP_INCREMENT 0.1

SYSTEM_MODE(SEMI_AUTOMATIC);

Adafruit_ILI9341 tft = Adafruit_ILI9341(A2, A1, A0);

int currentTemperature = 0;
int desiredTemperature = 0;
bool isHeatOn = false;
bool isFanOn = false;
bool motionDetected = false;

int lastChangedPot = -80;
int lastCurrentTemperature = 0;
int lastDesiredTemperature = 0;
bool lastIsHeatOn = false;
volatile int lastEncoded = 0;
volatile float encoderValue = 0;

void displayTemperature(void)
{
  bool forceRedraw = false || (lastChangedPot == -80);
  if (lastIsHeatOn != isHeatOn) forceRedraw = true;

  if (forceRedraw) {
    if (isHeatOn) tft.fillScreen(COLOR_HEATING);
    else tft.fillScreen(COLOR_COOLING);
  }

  if ((lastCurrentTemperature != currentTemperature)
      || (lastDesiredTemperature != desiredTemperature)
      || forceRedraw) {
    // Redraw scale
    tft.drawXBitmap(43, 13, scale_bits, scale_width, scale_height, ILI9341_WHITE);
  }

  if ((lastDesiredTemperature != desiredTemperature)
      || forceRedraw) {
    // Redraw desired temperature
    tft.setCursor(115, 95);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(8);
    tft.println(desiredTemperature);
  }

  if (((lastCurrentTemperature != currentTemperature)
      || forceRedraw) && currentTemperature > 0) {
    // Redraw current temperature
    tft.setCursor(140, 200);
    tft.setTextSize(4);
    tft.println(currentTemperature);
  }

  lastCurrentTemperature = currentTemperature;
  lastDesiredTemperature = desiredTemperature;
  lastIsHeatOn = isHeatOn;
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
  encoderValue = desiredTemperature;
  displayTemperature();
}

int setTemperature(int t)
{
  desiredTemperature = t;
  encoderValue = desiredTemperature;
  displayTemperature();
  // saveTemperature();
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

// Rotary encoder code by stahl
void updateEncoder() {
  int MSB = digitalRead(ENCODER_PIN_A); //MSB = most significant bit
  int LSB = digitalRead(ENCODER_PIN_B); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
      encoderValue -= TEMP_INCREMENT;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
      encoderValue += TEMP_INCREMENT;

  lastEncoded = encoded; //store this value for next time
  encoderValue = min(max(encoderValue, TEMP_MIN), TEMP_MAX);
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

  pinMode(FAN_PIN, OUTPUT);
  pinMode(HEAT_PIN, OUTPUT);

#ifdef USE_PIR
  pinMode(PIR_PIN, INPUT);
#endif

  Spark.connect();

  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  attachInterrupt(ENCODER_PIN_A, updateEncoder, CHANGE);
  attachInterrupt(ENCODER_PIN_B, updateEncoder, CHANGE);
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

  if (1000 == wait)
  {
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

  if (encoderValue != desiredTemperature) {
    setTemperature(encoderValue);
    Serial.print("Setting desired temp based on encoder to ");
    Serial.println(encoderValue);
  }

  isHeatOn = desiredTemperature > currentTemperature;
  digitalWrite(HEAT_PIN, isHeatOn);

  // just run them at the same time for now
  isFanOn = isHeatOn;
  digitalWrite(FAN_PIN, isFanOn);

  --wait;
}
