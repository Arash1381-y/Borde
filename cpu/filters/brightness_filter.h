//
// Created by halfblood on 7/6/23.
//

#ifndef CPU_BRIGHTNESS_FILTER_H
#define CPU_BRIGHTNESS_FILTER_H


typedef unsigned char ubyte;

int
change_brightness(const ubyte *gray_scaled_img, ubyte **brightness_changed_img, size_t width, size_t height,
                  size_t channels,
                  ubyte brightness_change);

#endif //CPU_BRIGHTNESS_FILTER_H
