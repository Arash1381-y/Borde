#ifndef BRIGHTNESS_FILTER_H
#define BRIGHTNESS_FILTER_H


typedef unsigned char ubyte;
typedef char byte;
typedef signed short int16;

#include <iostream>
#include <climits>

int
change_brightness(const ubyte *gray_scaled_img, ubyte **brightness_changed_img, size_t width, size_t height,
                  size_t channels,
                  byte brightness_change);

#endif //BRIGHTNESS_FILTER_H
