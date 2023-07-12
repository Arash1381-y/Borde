
#include "../filters/brightness_filter.h"


/**
 * Changes the brightness of a grayscale image by a specified amount.
 *
 * The input image is assumed to have a single grayscale channel per pixel,
 * and the output image is stored in a separate buffer allocated by this function.
 *
 * @param gray_scaled_img A pointer to the input grayscale image data.
 * @param brightness_changed_img A pointer to a buffer that will hold the resulting brightness-changed image data.
 * @param width The width of the image in pixels.
 * @param height The height of the image in pixels.
 * @param channels The number of color channels per pixel in the input image (should be 1 for grayscale images).
 * @param brightness_change The amount to change the brightness by, in the range [-128, 127].
 * @return 0 if the brightness change succeeded, or 1 if memory allocation failed or the input image has an invalid number of channels.
 */
int change_brightness(const ubyte *gray_scaled_img, ubyte **brightness_changed_img, size_t width, size_t height,
                      size_t channels,
                      byte brightness_change) {
    // Check if the input image and channels are valid
    if (gray_scaled_img == nullptr || channels != 1) {
        std::cout << "Invalid input image or number of channels. Expected a single-channel grayscale image.\n";
        return 1;
    }

    // Allocate memory for the brightness-changed image
    *brightness_changed_img = (ubyte *) malloc(width * height * sizeof(ubyte));

    // Check if the memory was allocated
    if (*brightness_changed_img == nullptr) {
        std::cout << "Failed to allocate memory for the brightness-changed image!\n";
        return 1;
    }

    // Change the brightness of the image
    for (int i = 0; i < width * height; i++) {
        // Get the pixel value
        ubyte pixel_color = gray_scaled_img[i];

        // Change the brightness of the pixel
        int16 r_ex = pixel_color;
        int16 b_ex = brightness_change;

        r_ex = (int16) (r_ex + b_ex);

        // Clip the pixel value to the range [0, 255]
        if (r_ex > UCHAR_MAX) {
            pixel_color = UCHAR_MAX;
        } else if (r_ex < 0) {
            pixel_color = 0;
        } else {
            pixel_color = r_ex & 0xFF;
        }

        // Set the pixel in the output buffer
        (*brightness_changed_img)[i] = pixel_color;
    }

    return 0;
}