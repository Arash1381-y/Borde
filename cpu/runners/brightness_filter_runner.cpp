#include <iostream>
#include <chrono>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../stb/stb_image.h"

#include "../stb/stb_image_write.h"

#include "../filters/brightness_filter.h"

#include "../../config.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    char *input_filename = (char *) malloc(sizeof(char) * FILENAME_MAX);
    char *result_path = (char *) malloc(sizeof(char) * (FILENAME_MAX + PATH_MAX));
    char *input_path = (char *) malloc(sizeof(char) * (FILENAME_MAX + PATH_MAX));
    byte brightness_change = BRIGHTNESS_DEFAULT;


    if (argc == 3) {
        strcpy(input_filename, argv[1]);
        // check if filename is png
        if (strstr(input_filename, ".png") == nullptr) {
            std::cout << "Invalid file type! Only png files are supported!\n";
            return 1;
        }

        // init the path with default path in config.h
        strcpy(input_path, INPUT_PATH);
        strcat(input_path, input_filename);

        // check if the path is valid and the file exists
        if (!fs::exists(input_path)) {
            std::cout << "Invalid file path!\n";
            return 1;
        }

        strcpy(result_path, RESULT_PATH);

        // remove the .png from the filename
        input_filename[strlen(input_filename) - 4] = '\0';

        strcat(result_path, input_filename);
        strcat(result_path, "bright.png");

        brightness_change = (byte) strtol(argv[2], nullptr, 10);

    } else {
        strcpy(input_path, INPUT_PATH);
        strcat(input_path, "input_bright.png");

        std::cout << "\033[1;31m" << "using default setup. input image : " << input_path << "\033[0m\n";

        // print the guide
        std::cout << "\033[1;31m" << "Usage : ./brightness_filter_runner <filename> <brightness_change>\n" << "\033[0m";

        // print the defaults
        std::cout << "\033[1;31m" << "Default filename : " << "input_bright.png" << "\n" << "\033[0m";
        std::cout << "\033[1;31m" << "Default brightness change : " << (int)brightness_change << "\n" << "\033[0m";

        // print the example
        std::cout << "\033[1;31m" << "Example : ./brightness_filter_runner input_bright.png 50\n" << "\033[0m";

        std::cout << "\033[1;31m" << "----------------------------------------\n" << "\033[0m";

        // check if the path is valid and the file exists
        if (!fs::exists(input_path)) {
            std::cout << "Invalid file path!\n";
            return 1;
        }

        strcpy(result_path, RESULT_PATH);
        strcat(result_path, "input_bright.png");
    }

    // load the image
    int width, height, channels;
    ubyte *image;
    image = stbi_load(input_path, &width, &height, &channels, 1);

    // start timer
    auto start = std::chrono::high_resolution_clock::now();

    // apply the filter
    // brightness change
    ubyte *brightness_changed_image;
    change_brightness(image,
                      &brightness_changed_image,
                      width, height,
                      1, brightness_change);


    // finish timer
    auto finish = std::chrono::high_resolution_clock::now();


    stbi_write_png(result_path, width, height,
                   1, brightness_changed_image, width);


    // print the time and the result path

    std::cout << "----------------------------------------\n";
    std::cout << "Brightness changing filter took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()
              << " milliseconds\n";
    std::cout << "----------------------------------------\n";

    std::cout << "\033[1;32m" << "Result saved in : " << result_path << "\033[0m\n";

    // free the memory
    free(input_filename);
    free(result_path);
    free(input_path);
    stbi_image_free(image);
    stbi_image_free(brightness_changed_image);

    return 0;
}
