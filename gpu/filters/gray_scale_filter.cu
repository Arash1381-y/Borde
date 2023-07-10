#include <iostream>
#include "gray_scale_filter.cuh"

#define GRID_SIZE 1024
#define BLOCK_SIZE 1024

#define CHANNELS_NUM 3

/**
 * Converts a color image to grayscale using CUDA kernel parallelization.
 *
 * The input image is assumed to have 3 color channels (red, green, and blue) per pixel,
 * and the output grayscale image is stored in a separate buffer allocated by the caller.
 *
 * @param image A pointer to the input image data.
 * @param gray_scaled_image A pointer to a buffer that will hold the resulting grayscale image data.
 * @param width The width of the image in pixels.
 * @param height The height of the image in pixels.
 */
__global__
static void convert_to_gray_scale_kernel(const ubyte *image, ubyte *gray_scaled_image, size_t width, size_t height) {
    // Calculate the index of the current pixel in the image
    size_t i = blockIdx.x * blockDim.x + threadIdx.x;

    // Only execute the kernel for valid pixel indices
    if (i < width * height) {
        // Calculate the index of the current pixel in the input image
        size_t image_index = i * CHANNELS_NUM;

        // Convert the pixel to grayscale using the specified weights for each channel
        gray_scaled_image[i] = (ubyte) (0.21 * image[image_index] +
                                       0.72 * image[image_index + 1] +
                                       0.07 * image[image_index + 2]);
    }
}

/**
 * Converts a color image to grayscale.
 *
 * The input image is assumed to have 3 color channels (red, green, and blue) per pixel.
 * The output grayscale image is stored in a separate buffer allocated by this function.
 *
 * @param image A pointer to the input image data.
 * @param gray_scaled_image A pointer to a buffer that will hold the resulting grayscale image data.
 * @param width The width of the image in pixels.
 * @param height The height of the image in pixels.
 * @param channels The number of color channels per pixel in the input image (should be 3 for RGB images).
 * @return 0 if the conversion succeeded, or 1 if memory allocation failed.
 */
int convert_to_gray_scale(const ubyte *image, ubyte **gray_scaled_image, size_t width, size_t height,
                          size_t channels) {


    // Check if the input image and channels are valid
    if (image == nullptr || channels != 3) {
        std::cout << "Invalid input image or number of channels. Expected a 3-channel RGB image.\n";
        return 1;
    }


    // allocate memory for the gray scaled image
    *gray_scaled_image = (ubyte *) malloc(width * height * sizeof(ubyte));

    // allocate memory for the image in the device
    ubyte *d_image;
    cudaMalloc((void **) &d_image, width * height * channels * sizeof(ubyte));

    // allocate memory for the gray scaled image in the device
    ubyte *d_gray_scaled_image;
    cudaMalloc((void **) &d_gray_scaled_image, width * height * sizeof(ubyte));

    // copy the image to the device
    cudaMemcpy(d_image, image, width * height * channels * sizeof(ubyte), cudaMemcpyHostToDevice);

    // calculate the number of blocks and threads
    size_t number_of_blocks = width * height / BLOCK_SIZE;
    if ((width * height) % BLOCK_SIZE != 0) {
        number_of_blocks++;
    }

    if (number_of_blocks > GRID_SIZE) {
        number_of_blocks = GRID_SIZE;
    }

    // call the kernel
    convert_to_gray_scale_kernel<<<number_of_blocks, BLOCK_SIZE>>>
            (
                    d_image, d_gray_scaled_image,
                    width, height
            );

    // copy the gray scaled image to the host
    cudaMemcpy(*gray_scaled_image, d_gray_scaled_image, width * height * sizeof(ubyte),
               cudaMemcpyDeviceToHost);

    // free the memory
    cudaFree(d_image);
    cudaFree(d_gray_scaled_image);


    return 0;
}
