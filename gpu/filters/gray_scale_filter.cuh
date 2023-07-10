//
// Created by halfblood on 7/6/23.
//

#ifndef GPU_GRAY_SCALE_FILTER_CUH
#define GPU_GRAY_SCALE_FILTER_CUH

#include <cstdio>

typedef unsigned char ubyte;

int
convert_to_gray_scale(const ubyte *image, ubyte **gray_scaled_image, size_t width, size_t height,
                      size_t channels);

#endif //GPU_GRAY_SCALE_FILTER_CUH
