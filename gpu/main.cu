#include <iostream>
#include <chrono>


#include "../stb/stb_image.h"

#include "../stb/stb_image_write.h"

#include "../config.h"

#define STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "filters/gray_scale_filter.cuh"

#include "filters/brightness_filter.cuh"

#include "filters//sobel_filter.cuh"


void do_gray_scale_test(ubyte *input, ubyte *output, size_t width, size_t height) {

    // convert to gray
    convert_to_gray_scale(input, &output, width, height, 3);

    // write the gray scaled image
    char *output_path = (char *) malloc((MAX_FILE_NAME + MAX_PATH_NAME) * sizeof(char));
    sprintf(output_path, "%s%s", INPUT_PATH, "gray_scaled.png");

    stbi_write_png(output_path, width, height,
                   1, output, width);
}

void do_brightness_test(ubyte *input, ubyte *output, size_t width, size_t height, byte br) {
    change_brightness(input,
                      &output,
                      width, height,
                      1, br);


    char *output_path = (char *) malloc((MAX_FILE_NAME + MAX_PATH_NAME) * sizeof(char));
    sprintf(output_path, "%s%s", INPUT_PATH, "brightness_changed.png");

    // write the brightness changed image
    stbi_write_png(output_path, width, height,
                   1, output, width);
}

void init_arg(){

}

int main(int argc, char *argv[]) {

    // args
    bool default_mode;
    char *input_path = (char *) malloc((MAX_FILE_NAME + MAX_PATH_NAME) * sizeof(char));
    bool change_brightness;






    // load the image
    char file_name[MAX_FILE_NAME] = "input.png";
    sprintf(input_path, "%s%s", INPUT_PATH, file_name);

    int width, height, channels;
    unsigned char *image = stbi_load(input_path, &width, &height, &channels, 3);

    if (image == nullptr) {
        printf("Error in loading the image\n");
        return 1;
    }

    // set timer start with chrono
    auto start = std::chrono::high_resolution_clock::now();


    // gray scales the image
    unsigned char *gray_scaled_image;
    do_gray_scale_test(image, gray_scaled_image, width, height);

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
