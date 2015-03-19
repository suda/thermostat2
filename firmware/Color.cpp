#include "Color.h"

Color::Color(uint8_t r, uint8_t g, uint8_t b) {
  red = r;
  green = g;
  blue = b;
}

uint16_t Color::toColor565() {
  return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}

uint8_t Color::getRed() {
  return red;
}

uint8_t Color::getGreen(){
  return green;
}

uint8_t Color::getBlue(){
  return blue;
}

Color Color::applyAlpha(Color background, float alpha) {
  uint8_t r = alpha * red + (1.0 - alpha) * background.getRed();
  uint8_t g = alpha * green + (1.0 - alpha) * background.getGreen();
  uint8_t b = alpha * blue + (1.0 - alpha) * background.getBlue();

  return Color(r, g, b);
}
