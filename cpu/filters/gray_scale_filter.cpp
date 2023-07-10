#include "gray_scale_filter.h"


#include <iostream>
#include "../stb/stb_image.h"
#include "../stb/stb_image_write.h"
#include "../stb/stb_image.h"


#define CHANNELS_NUM 3


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

    // Allocate memory for the grayscale image
    *gray_scaled_image = (ubyte *) malloc(width * height * sizeof(ubyte));

    // Check if the memory was allocated
    if (*gray_scaled_image == nullptr) {
        std::cout << "Failed to allocate memory for the grayscale image!\n";
        return 1;
    }

    // Convert the image to grayscale
    for (int i = 0; i < width * height; i++) {
        // Get the pixel values for each channel
        size_t image_index = CHANNELS_NUM * i;
        ubyte r = image[image_index];
        ubyte g = image[image_index + 1];
        ubyte b = image[image_index + 2];

        // Convert the pixel to grayscale using the specified weights for each channel
        auto gray_pixel = (ubyte) (0.21 * r + 0.72 * g + 0.07 * b);

        // Store the grayscale pixel value in the output buffer
        (*gray_scaled_image)[i] = gray_pixel;
    }

    return 0;
}
