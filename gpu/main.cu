#include <iostream>
#include <chrono>


#define STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image.h"

#include "stb/stb_image_write.h"

#include "filters/gray_scale_filter.cuh"

#include "filters/brightness_filter.cuh"

#include "filters//sobel_filter.cuh"


int main() {
    // load the image
    int width, height, channels;
    unsigned char *image = stbi_load("./samples/img.png", &width, &height, &channels, 3);

    if (image == nullptr) {
        printf("Error in loading the image\n");
        return 1;
    }

    // set timer start with chrono
    auto start = std::chrono::high_resolution_clock::now();


    // gray scales the image
    unsigned char *gray_scaled_image;
    convert_to_gray_scale(image, &gray_scaled_image, width, height, 3);



    // write the gray scaled image
    stbi_write_png("./results/gray_scaled_image.png", width, height,
                   1, gray_scaled_image, width);

    // brightness change
    unsigned char *brightness_changed_image;
    change_brightness(gray_scaled_image,
                      &brightness_changed_image,
                      width, height,
                      1, 5);

    // write the brightness changed image
    stbi_write_png("./results/brightness_changed_image.png", width, height,
                   1, brightness_changed_image, width);

    // sobel x dir filter
    unsigned char *sobel_x_dir_image;
    detect_edges(brightness_changed_image,
                 &sobel_x_dir_image,
                 width, height,
                 75, .2, 0);

    // write the sobel x dir image
    stbi_write_png("./results/sobel_x_dir_image.png", width, height,
                   1, sobel_x_dir_image, width);



    // sobel y dir filter
    unsigned char *sobel_y_dir_image;
    detect_edges(
            brightness_changed_image,
            &sobel_y_dir_image,
            width, height,
            75, .2, 1);

    // write the sobel y dir image
    stbi_write_png("./results/sobel_y_dir_image.png", width, height,
                   1, sobel_y_dir_image, width);

    // sobel x and y dir filter
    unsigned char *sobel_x_y_dir_image;
    detect_edges(
            gray_scaled_image,
            &sobel_x_y_dir_image,
            width, height,
            75, .1, 2);

    // write the sobel x and y dir image
    stbi_write_png("./results/sobel_x_y_dir_image.png", width, height,
                   1, sobel_x_y_dir_image, width);

    // free the memory
    stbi_image_free(image);
    stbi_image_free(gray_scaled_image);
    stbi_image_free(brightness_changed_image);
    stbi_image_free(sobel_x_dir_image);
    stbi_image_free(sobel_y_dir_image);
    stbi_image_free(sobel_x_y_dir_image);

    // set a timer end with chrono
    auto end = std::chrono::high_resolution_clock::now();

    // calculate the time difference
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // print in milliseconds
    std::cout << duration.count() / 1000.0 << " ms" << std::endl;

    return 0;
}
