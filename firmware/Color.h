#include "Adafruit_ILI9341.h"

class Color{
private:
  uint8_t red;
  uint8_t green;
  uint8_t blue;

public:
  Color(uint8_t r, uint8_t g, uint8_t b);

  uint16_t toColor565();
  uint8_t getRed();
  uint8_t getGreen();
  uint8_t getBlue();
  Color applyAlpha(Color background, float alpha);
};
