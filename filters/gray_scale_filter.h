//
// Created by halfblood on 7/6/23.
//

#ifndef CPU_GRAY_SCALE_FILTER_H
#define CPU_GRAY_SCALE_FILTER_H

#include <cstdio>

typedef unsigned char ubyte;

int
convert_to_gray_scale(const ubyte *image, ubyte **gray_scaled_image, size_t width, size_t height,
                      size_t channels);


#endif //CPU_GRAY_SCALE_FILTER_H
