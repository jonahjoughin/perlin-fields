#include <math.h>

#include "color.h"

RGB hsvToRGB(HSV hsv) {
  double r = 0, g = 0, b = 0;
  hsv.h*=360;
  if (hsv.s == 0)
	{
		r = hsv.v;
		g = hsv.v;
		b = hsv.v;
	}
	else
	{
		int i;
		double f, p, q, t;
		if (hsv.h == 360)
			hsv.h = 0;
		else
			hsv.h /= 60;

		i = (int)trunc(hsv.h);
		f = hsv.h - i;

		p = hsv.v * (1.0 - hsv.s);
		q = hsv.v * (1.0 - (hsv.s * f));
		t = hsv.v * (1.0 - (hsv.s * (1.0 - f)));

		switch (i)
		{
		case 0:
			r = hsv.v;
			g = t;
			b = p;
			break;

		case 1:
			r = q;
			g = hsv.v;
			b = p;
			break;

		case 2:
			r = p;
			g = hsv.v;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = hsv.v;
			break;

		case 4:
			r = t;
			g = p;
			b = hsv.v;
			break;

		default:
			r = hsv.v;
			g = p;
			b = q;
			break;
		}
  }
  RGB rgb =  {
    r,
    g,
    b
  };
  return rgb;
}
