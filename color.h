#ifndef COLOR_H
#define COLOR_H

typedef struct HSV {
  double h;
  double s;
  double v;
} HSV;

typedef struct RGB {
  double r;
  double g;
  double b;
} RGB;

RGB hsvToRGB(HSV hsv);

#endif
