#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <climits>

#ifndef SOBEL_FILTER_H
#define SOBEL_FILTER_H


typedef char byte;
typedef unsigned char ubyte;


int detect_edges(const ubyte *image, ubyte **edges_detected_image, size_t width, size_t height,
                 ubyte threshold,
                 double strength_ratio,
                 short dir);

#endif //SOBEL_FILTER_H
