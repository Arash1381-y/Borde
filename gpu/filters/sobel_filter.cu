#include <assert.h>
#include <iostream>
#include "sobel_filter.cuh"


#define KERNEL_WIDTH 3
#define KERNEL_HEIGHT 3


__device__
static ubyte strength_edge(long long value,
                           ubyte threshold,
                           double effect_ratio);

__device__
static ubyte clip_to_ubyte(long long val);

__device__
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
__device__
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


__global__
void gpu_operate_2d_conv(ubyte *image,
                         ubyte *out_image,
                         size_t height, size_t width,
                         byte *kernel,
                         ubyte threshold) {

    size_t i_index = blockIdx.y * blockDim.y + threadIdx.y;
    size_t j_index = blockIdx.x * blockDim.x + threadIdx.x;

    if (i_index < height && j_index < width) {
        ubyte img_sec[KERNEL_HEIGHT * KERNEL_WIDTH]; // section of image (separate by channel)

        extract_kernel(image,
                       img_sec,
                       height, width,
                       KERNEL_HEIGHT, KERNEL_WIDTH,
                       i_index, j_index);

        convolve_2d(KERNEL_HEIGHT, KERNEL_WIDTH,
                    img_sec, kernel,
                    &out_image[i_index * width + j_index]);
    }
}

/**
 * CUDA kernel that applies the Sobel filter to an input image to detect edges.
 *
 * @param image: Pointer to the input image data stored as an array of unsigned bytes (`ubyte`).
 * @param out_image: Pointer to the output image data that will contain the detected edges.
 * @param height: The height of the input image, in pixels.
 * @param width: The width of the input image, in pixels.
 * @param Gx: Pointer to the Sobel filter kernel for the horizontal direction.
 * @param Gy: Pointer to the Sobel filter kernel for the vertical direction.
 * @param apply_threshold: A boolean flag indicating whether to apply the threshold to the edge strength values.
 * @param threshold: A threshold value used to filter out weak edges.
 * @param strength_ratio: A ratio used to adjust the strength of the edge detection.
 */
__global__
static void detect_edges_sobel(const ubyte *image,
                               ubyte *out_image,
                               size_t height, size_t width,
                               const byte *Gx, const byte *Gy,
                               bool apply_threshold,
                               ubyte threshold,
                               double strength_ratio) {

    size_t row_index = blockIdx.y * blockDim.y + threadIdx.y;
    size_t col_index = blockIdx.x * blockDim.x + threadIdx.x;

    if (row_index < height && col_index < width) {
        // Extract the kernel centered at the current pixel
        ubyte kernel_sec[KERNEL_HEIGHT * KERNEL_WIDTH], x_c, y_c;
        extract_kernel(image,
                       kernel_sec,
                       height, width,
                       KERNEL_HEIGHT, KERNEL_WIDTH,
                       row_index, col_index);

        // Convolve the kernel with the Sobel filter kernels to calculate the horizontal and vertical gradients
        convolve_2d(KERNEL_HEIGHT, KERNEL_WIDTH,
                    kernel_sec, Gx,
                    &x_c);

        convolve_2d(KERNEL_HEIGHT, KERNEL_WIDTH,
                    kernel_sec, Gy,
                    &y_c);

        // Calculate the edge strength using the gradient magnitudes
        auto edge_strength = (long long) hypotf(x_c, y_c);

        // Apply the threshold to filter out weak edges if required
        if (apply_threshold) {
            out_image[row_index * width + col_index] = strength_edge(edge_strength, threshold, strength_ratio);
        } else {
            out_image[row_index * width + col_index] = clip_to_ubyte(edge_strength);
        }
    }
}

/**
 * Detect edges in an input image using the Sobel filter.
 *
 * @param image: A pointer to the input image data, stored as an array of unsigned bytes (`ubyte`).
 * @param edges_detected_image: A pointer to a pointer that will be used to store the output image data, which will
 * contain the detected edges.
 * @param width: The width of the input image, in pixels.
 * @param height: The height of the input image, in pixels.
 * @param threshold: A threshold value used to filter out weak edges.
 * @param strength_ratio: A ratio used to adjust the strength of the edge detection.
 * @param dir: A flag that determines the direction of the edge detection. If `dir` is 0, the function will detect edges
 * in the horizontal direction, if it is 1, it will detect edges in the vertical direction, and if it is any other value,
 * it will detect edges in both directions.
 *
 * @return: Returns 0 if the function executed successfully, and 1 if there was an error (such as an invalid input image
 * or failure to allocate memory for the output image).
 */
int detect_edges(const ubyte *image,
                 ubyte **edges_detected_image,
                 size_t width, size_t height,
                 ubyte threshold,
                 double strength_ratio,
                 short dir) {

    // Check if the input image is valid
    if (image == nullptr) {
        std::cout << "Invalid input image\n";
        return 1;
    }

    // Initialize the output image
    *edges_detected_image = (ubyte *) malloc(width * height * sizeof(ubyte));
    if (*edges_detected_image == nullptr) return 1;

    // Initialize the Sobel filter kernels
    byte Gx[KERNEL_HEIGHT * KERNEL_WIDTH] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    byte Gy[KERNEL_HEIGHT * KERNEL_WIDTH] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

    // Initialize the device memory
    ubyte *d_image, *d_out_image;
    byte *d_Gx, *d_Gy;
    cudaMalloc(&d_image, width * height * sizeof(ubyte));
    cudaMalloc(&d_out_image, width * height * sizeof(ubyte));
    cudaMalloc(&d_Gx, KERNEL_WIDTH * KERNEL_HEIGHT * sizeof(byte));
    cudaMalloc(&d_Gy, KERNEL_WIDTH * KERNEL_HEIGHT * sizeof(byte));

    // Copy the input image and filter kernels to the device memory
    cudaMemcpy(d_image, image, width * height * sizeof(ubyte), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Gx, Gx, KERNEL_HEIGHT * KERNEL_HEIGHT * sizeof(byte), cudaMemcpyHostToDevice);
    cudaMemcpy(d_Gy, Gy, KERNEL_HEIGHT * KERNEL_HEIGHT * sizeof(byte), cudaMemcpyHostToDevice);

    // Calculate the number of blocks and threads to use
    dim3 block_size(32, 32);
    dim3 grid_size((width + block_size.x - 1) / block_size.x, (height + block_size.y - 1) / block_size.y);

    // Launch the CUDA kernel to detect edges
    if (dir == 0)
        gpu_operate_2d_conv<<<grid_size, block_size>>>
                (d_image,
                 d_out_image,
                 height, width,
                 d_Gx, threshold);
    else if (dir == 1)
        gpu_operate_2d_conv<<<grid_size, block_size>>>
                (d_image,
                 d_out_image,
                 height, width,
                 d_Gy, threshold);
    else
        detect_edges_sobel<<<grid_size, block_size>>>
                (d_image,
                 d_out_image,
                 height, width,
                 d_Gx, d_Gy,
                 true, threshold, strength_ratio);

    // Copy the output image back to the host memory
    cudaMemcpy(*edges_detected_image, d_out_image, width * height * sizeof(ubyte), cudaMemcpyDeviceToHost);

    // Free the device memory
    cudaFree(d_image);
    cudaFree(d_out_image);
    cudaFree(d_Gx);
    cudaFree(d_Gy);
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
__device__
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
__device__
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
__device__
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