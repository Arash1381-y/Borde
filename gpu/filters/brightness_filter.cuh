//
// Created by halfblood on 7/6/23.
//

#ifndef GPU_BRIGHTNESS_FILTER_CUH
#define GPU_BRIGHTNESS_FILTER_CUH

typedef unsigned char ubyte;


int
change_brightness(const ubyte *gray_scaled_img, ubyte **brightness_changed_img, size_t width, size_t height,
                  size_t channels,
                  int brightness_change);

#endif //GPU_BRIGHTNESS_FILTER_CUH
