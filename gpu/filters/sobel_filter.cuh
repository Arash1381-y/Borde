//
// Created by halfblood on 7/6/23.
//

#ifndef GPU_SOBEL_FILTER_CUH
#define GPU_SOBEL_FILTER_CUH


typedef unsigned char ubyte;
typedef char byte;


int detect_edges(const ubyte *image,
                 ubyte **edges_detected_image,
                 size_t width, size_t height,
                 ubyte threshold,
                 double strength_ratio,
                 short dir);

#endif //GPU_SOBEL_FILTER_CUH

