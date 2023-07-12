#include <iostream>
#include <chrono>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../stb/stb_image.h"

#include "../stb/stb_image_write.h"

#include "../filters/brightness_filter.h"

#include "../config.h"

#include "helper.cpp"

namespace fs = std::filesystem;

void guide() {
    std::cout << "\033[1;33m" << "----------------------------------------\n" << "\033[0m";

    std::cout << "\033[1;33m" << "GUIDE: " << "\033[0m\n";

    std::cout << "\033[1;33m" << "No arguments were provided! Default values will be used!" << "\033[0m\n";
    std::cout << "\033[1;33m"
              << "Usage: ./brightness_filter_runner.out <input_path> <input_filename> <result_path> <brightness_change>"
              << "\033[0m\n";
    std::cout << "\033[1;33m" << "Default values: " << "\033[0m\n";
    std::cout << "\033[1;33m" << "input_path: " << DEFAULT_INPUT_PATH << "\033[0m\n";
    std::cout << "\033[1;33m" << "input_filename: " << DEFAULT_INPUT_FILENAME << "\033[0m\n";
    std::cout << "\033[1;33m" << "result_path: " << DEFAULT_RESULT_PATH << "\033[0m\n";
    std::cout << "\033[1;33m" << "brightness_change: " << (int) BRIGHTNESS_DEFAULT << "\033[0m\n";

    std::cout << "\033[1;33m" << "Use '-' as an argument to use the default value!" << "\033[0m\n";

    std::cout << "\033[1;33m" << "Example: ./brightness_filter_runner.out - - - 50" << "\033[0m\n";

    std::cout << "\033[1;33m" << "----------------------------------------\n" << "\033[0m\n";
}

int main(int argc, char *argv[]) {

    char *input_filename = (char *) malloc(sizeof(char) * FILENAME_MAX);
    char *result_path = (char *) malloc(sizeof(char) * (FILENAME_MAX + PATH_MAX));
    char *input_path = (char *) malloc(sizeof(char) * (FILENAME_MAX + PATH_MAX));
    byte brightness_change;


    if (argc == 5) {
        SET_OR_DEFAULT(argv[1], input_path, DEFAULT_INPUT_PATH)
        SET_OR_DEFAULT(argv[2], input_filename, DEFAULT_INPUT_FILENAME)
        SET_OR_DEFAULT(argv[3], result_path, DEFAULT_RESULT_PATH)

        if (!IS_PNG(input_filename)) { ERROR_COUT_AND_RETURN(INVALID_FILE_TYPE) }

        // concat the path and filename
        strcat(input_path, input_filename);

        if (!PATH_EXISTS(input_path)) { ERROR_COUT_AND_RETURN(INVALID_FILE_PATH) }
        if (!PATH_EXISTS(result_path)) { ERROR_COUT_AND_RETURN(INVALID_RESULT_PATH) }

        // concat the result path and filename without the .png
        input_filename[strlen(input_filename) - 4] = '\0';
        strcat(result_path, input_filename);
        strcat(result_path, "_bright.png");

        // the fourth arg is the brightness change
        brightness_change = (byte) strtol(argv[4], nullptr, 10);
        if (brightness_change < 0 || brightness_change > 127) { ERROR_COUT_AND_RETURN(INVALID_BRIGHTNESS_CHANGE) }

    } else if (argc == 1) {
        // use default values
        strcpy(input_path, DEFAULT_INPUT_PATH);
        strcpy(input_filename, DEFAULT_INPUT_FILENAME);
        strcat(input_path, input_filename);
        strcpy(result_path, DEFAULT_RESULT_PATH);
        brightness_change = BRIGHTNESS_DEFAULT;


        if (!IS_PNG(input_filename)) { ERROR_COUT_AND_RETURN(INVALID_FILE_TYPE) }
        if (!PATH_EXISTS(input_path)) { ERROR_COUT_AND_RETURN(INVALID_FILE_PATH) }
        if (!PATH_EXISTS(result_path)) { ERROR_COUT_AND_RETURN(INVALID_RESULT_PATH) }
        if (brightness_change < 0 || brightness_change > 127) { ERROR_COUT_AND_RETURN(INVALID_BRIGHTNESS_CHANGE) }

        // remove the .png from the filename
        input_filename[strlen(input_filename) - 4] = '\0';
        strcat(result_path, input_filename);
        strcat(result_path, "_bright.png");

        guide();

    } else {
        ERROR_COUT_AND_RETURN(INVALID_ARGUMENTS)
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


    std::cout << "\033[1;34m" << "----------------------------------------\n" << "\033[0m";
    std::cout << "\033[1;34m" << "REPORT: " << "\033[0m\n";

    std::cout << "\033[1;34m" << "Time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()
              << "ms\n" << "\033[0m";
    std::cout << "\033[1;34m" << "----------------------------------------\n" << "\033[0m\n";

    std::cout << "\033[1;32m" << "----------------------------------------\n" << "\033[0m";
    std::cout << "\033[1;32m" << "RESULT: " << "\033[0m\n";
    std::cout << "\033[1;32m" << "Result saved in : " << result_path << "\033[0m\n";
    std::cout << "\033[1;32m" << "----------------------------------------\n" << "\033[0m\n";

    // free the memory
    free(input_filename);
    free(result_path);
    free(input_path);
    stbi_image_free(image);
    stbi_image_free(brightness_changed_image);

    return 0;
}
