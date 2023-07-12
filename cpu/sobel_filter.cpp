#include <iostream>
#include "../filters/sobel_filter.h"


#define KERNEL_WIDTH 3
#define KERNEL_HEIGHT 3

static ubyte strength_edge(long long value,
                           ubyte threshold,
                           double effect_ratio);

static ubyte clip_to_ubyte(long long val);


static void extract_kernel(const ubyte *input,
                           ubyte *output,
                           size_t height, size_t width,
                           size_t kernel_height, size_t kernel_width,
                           size_t i_index, size_t j_index);


/**
 * Applies a 2D convolution to an input image using
 *
 * @param n_row number of kernel rows
 * @param n_col number of kernel cols
 * @param input input array
 * @param kernel kernel array
 * @param output result of convolution
 */
static void convolve_2d(size_t n_row, size_t n_col,
                        const ubyte *input,
                        const byte *kernel,
                        ubyte *output) {
    long long temp = 0;

    // error checks
    assert(n_row * n_col < ULONG_LONG_MAX && "The kernel size is not supported !");

    for (size_t i = 0; i < n_row * n_col; i++) {
        temp += kernel[i] * input[i];
    }

    *output = clip_to_ubyte(temp);
}

/**
 * Applies Sobel Operation for detecting the edges
 *
 * @param n_row number of kernel rows
 * @param n_col number of kernel cols
 * @param img_sec image section
 * @param output color of pixel
 * @param Gx x dir Filter
 * @param Gy y dir Filter
 * @param apply_threshold if ture, use threshold to weaken or strengthen the edge
 * @param threshold threshold value
 */
static void sobel(size_t n_row, size_t n_col,
                  ubyte *img_sec, ubyte *output,
                  byte *Gx, byte *Gy,
                  bool apply_threshold,
                  unsigned char threshold,
                  double strength_ratio) {

    ubyte x, y;
    long long temp;
    convolve_2d(n_row, n_col, img_sec, Gx, &x);
    convolve_2d(n_row, n_col, img_sec, Gy, &y);


    temp = (long long) hypot(x, y);

    if (apply_threshold) *output = strength_edge(temp, threshold, strength_ratio);
    else *output = clip_to_ubyte(temp);
}

/**
 * Detect Edge by using Sobel Operation 
 * @param image input image
 * @param edges_detected_image output image
 * @param width width of input image
 * @param height width of output image
 * @param threshold threshold to apply
 * @param dir direction of edge detection 
 * ( 0 : only vertical edges , 1 : only horizontal edges, 2: horizontal and vertical edges)
 * @return 1 if any error occurs
 */
int detect_edges(const ubyte *image,
                 ubyte **edges_detected_image,
                 size_t width, size_t height,
                 ubyte threshold,
                 double strength_ratio,
                 short dir) {

    // Check if the input image and channels are valid
    if (image == nullptr) {
        std::cout << "Invalid input image\n";
        return 1;
    }

    byte Gx[KERNEL_HEIGHT * KERNEL_WIDTH] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};

    byte Gy[KERNEL_HEIGHT * KERNEL_WIDTH] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};


    ubyte img_sec[KERNEL_HEIGHT * KERNEL_WIDTH]; // section of image (separate by channel)

    ubyte out_pixel;
    *edges_detected_image = (ubyte *) malloc(width * height * sizeof(ubyte));

    // check if the memory was allocated
    if (*edges_detected_image == nullptr) {
        std::cout << "Failed to allocate memory for the edge detected image!\n";
        return 1;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            extract_kernel(image, img_sec,
                           height, width,
                           KERNEL_HEIGHT, KERNEL_WIDTH,
                           i, j);

            switch (dir) {
                case 0:
                    convolve_2d(KERNEL_HEIGHT, KERNEL_WIDTH,
                                img_sec, Gx,
                                &out_pixel);
                    break;
                case 1:
                    convolve_2d(KERNEL_HEIGHT, KERNEL_WIDTH,
                                img_sec, Gy,
                                &out_pixel);
                    break;

                case 2:
                default:
                    sobel(KERNEL_HEIGHT, KERNEL_WIDTH,
                          img_sec, &out_pixel,
                          Gx, Gy,
                          true, threshold, strength_ratio);

            }

            size_t index = (i * width + j);
            (*edges_detected_image)[index] = out_pixel;
        }
    }
    return 0;
}

/**
 * Adjusts an input value based on a threshold and a strength ratio.
 *
 * If the input value is greater than the threshold, it is increased by a factor of
 * (1 + strength_ratio). Otherwise, it is decreased by a factor of (1 - strength_ratio).
 *
 * @param value The input value to adjust.
 * @param threshold The threshold for comparing the input value.
 * @param strength_ratio The strength ratio used to increase or decrease the input value.
 * @return The adjusted input value, clipped to the range [0, 255].
 */
static ubyte strength_edge(long long value, ubyte threshold, double strength_ratio) {
    if (value > threshold)
        return clip_to_ubyte((long long) ((double) value * (1 + strength_ratio)));
    else
        return clip_to_ubyte((long long) ((double) value * (1 - strength_ratio)));
}


/**
 * Clips a given long long value to the range of an unsigned byte (0 to 255).
 *
 * @param val The value to clip.
 * @return The clipped value, truncated to an unsigned byte in the range [0, 255].
 */
static ubyte clip_to_ubyte(long long val) {
    if (val > UCHAR_MAX || val < -1 * UCHAR_MAX) {
        return UCHAR_MAX;
    } else {
        return abs((int) val);
    }
}

/**
 * Extracts a sub-matrix from an input matrix centered at the given i and j indices.
 *
 * The sub-matrix is extracted using a kernel of the given height and width, and
 * the resulting sub-matrix is stored in the output array.
 *
 * If any part of the kernel extends beyond the bounds of the input matrix, the
 * corresponding elements of the output array are set to zero.
 *
 * @param input A pointer to the input matrix data.
 * @param output A pointer to the output sub-matrix data.
 * @param height The number of rows in the input matrix.
 * @param width The number of columns in the input matrix.
 * @param kernel_height The height of the kernel used to extract the sub-matrix.
 * @param kernel_width The width of the kernel used to extract the sub-matrix.
 * @param i_index The row index at the center of the sub-matrix.
 * @param j_index The column index at the center of the sub-matrix.
 */
static void extract_kernel(const ubyte *input,
                           ubyte *output,
                           size_t height, size_t width,
                           size_t kernel_height, size_t kernel_width,
                           size_t i_index, size_t j_index) {

    long i_dist, j_dist, dist_ij;
    dist_ij = ((long) kernel_width - 1) / 2;

    for (int i = 0; i < kernel_width; i++) {
        i_dist = (long) i_index + i - dist_ij;
        for (int j = 0; j < kernel_height; j++) {
            j_dist = (long) j_index + j - dist_ij;

            // Check if the current index is out of bounds
            if (i_dist >= height || i_dist < 0 || j_dist >= width || j_dist < 0) {
                output[i * kernel_width + j] = 0;
            } else {
                // Compute the index of the current element in the input matrix
                size_t index = i_dist * width + j_dist;
                output[i * kernel_width + j] = input[index];
            }
        }
    }
}