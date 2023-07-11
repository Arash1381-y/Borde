#include <iostream>
#include <chrono>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../stb/stb_image.h"

#include "../stb/stb_image_write.h"

#include "../filters/gray_scale_filter.h"

#include "../../config.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    char *input_filename = (char *) malloc(sizeof(char) * FILENAME_MAX);
    char *result_path = (char *) malloc(sizeof(char) * (FILENAME_MAX + PATH_MAX));
    char *input_path = (char *) malloc(sizeof(char) * (FILENAME_MAX + PATH_MAX));


    if (argc == 2) {
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

        // remove the .png extension
        input_filename[strlen(input_filename) - 4] = '\0';

        strcat(result_path, input_filename);
        strcat(result_path, "_gray_scaled.png");

    } else {
        std::cout << "\033[1;31m" << "No arguments provided! Using default setup!\n" << "\033[0m";

        // GUIDE
        std::cout << "\033[1;31m" << "Usage : ./gray_scale_runner <filename>\n" << "\033[0m";

        // DEFAULTS
        std::cout << "\033[1;31m" << "Default filename : " << "input_gray.png" << "\n" << "\033[0m";

        // EXAMPLE
        std::cout << "\033[1;31m" << "Example : ./gray_scale_runner input_gray.png\n" << "\033[0m";

        std::cout << "\033[1;31m" << "----------------------------------------\n" << "\033[0m";


        strcpy(input_path, INPUT_PATH);
        strcat(input_path, "input_gray.png");

        // print the path in yellow
        std::cout << "\033[1;33m" << "using default setup. input image : " << input_path << "\033[0m\n";

        // check if the path is valid and the file exists
        if (!fs::exists(input_path)) {
            std::cout << "Invalid file path!\n";
            return 1;
        }

        strcpy(result_path, RESULT_PATH);
        strcat(result_path, "input_gray_scaled.png");
    }

    // load the image
    int width, height, channels;
    ubyte *image;
    image = stbi_load(input_path, &width, &height, &channels, 3);

    // start timer
    auto start = std::chrono::high_resolution_clock::now();

    // gray scales the image
    ubyte *gray_scaled_image;
    int state = convert_to_gray_scale(image, &gray_scaled_image, width, height, 3);
    if (state == 1) {
        std::cout << "Error while converting to gray scale!\n";
        return 1;
    }

    // finish timer
    auto finish = std::chrono::high_resolution_clock::now();


    stbi_write_png(result_path, width, height,
                   1, gray_scaled_image, width);


    // print the time and the result path

    std::cout << "----------------------------------------\n";
    std::cout << "Gray scale filter took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()
              << " milliseconds\n";
    std::cout << "----------------------------------------\n";

    std::cout << "\033[1;32m" << "Result saved in : " << result_path << "\033[0m\n";
    std::cout << "\033[1;32m" << "----------------------------------------\n" << "\033[0m";

    // free the memory
    free(input_filename);
    free(result_path);
    free(input_path);
    stbi_image_free(image);
    stbi_image_free(gray_scaled_image);

    return 0;

}
