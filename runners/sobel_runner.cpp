
#include <iostream>
#include <chrono>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION


#include "../stb/stb_image.h"

#include "../stb/stb_image_write.h"

#include "../filters/sobel_filter.h"

#include "../config.h"

#include "helper.cpp"

namespace fs = std::filesystem;


void guide() {
    std::cout << "\033[1;33m" << "----------------------------------------\n" << "\033[0m";

    std::cout << "\033[1;33m" << "GUIDE: " << "\033[0m\n";

    std::cout << "\033[1;33m" << "No arguments were provided! Default values will be used!" << "\033[0m\n";
    std::cout << "\033[1;33m"
              << "Usage: ./sobel_filter_runner.out <input_path> <input_filename> <result_path> <threshold> <scale>"
              << "\033[0m\n";

    std::cout << "\033[1;33m" << "Default values: " << "\033[0m\n";
    std::cout << "\033[1;33m" << "input_path: " << DEFAULT_INPUT_PATH << "\033[0m\n";
    std::cout << "\033[1;33m" << "input_filename: " << DEFAULT_INPUT_FILENAME << "\033[0m\n";
    std::cout << "\033[1;33m" << "result_path: " << DEFAULT_RESULT_PATH << "\033[0m\n";
    std::cout << "\033[1;33m" << "threshold: " << SOBEL_THRESHOLD << "\033[0m\n";
    std::cout << "\033[1;33m" << "scale: " << STRENGTH_RATIO << "\033[0m\n";
    std::cout << "\033[1;33m" << "Example: ./sobel_filter_runner.out - - - 50 0.3" << "\033[0m\n";

    std::cout << "\033[1;33m" << "----------------------------------------\n" << "\033[0m\n";
}

int main(int argc, char *argv[]) {

    char *input_filename = (char *) malloc(sizeof(char) * FILENAME_MAX);
    char *result_path = (char *) malloc(sizeof(char) * (FILENAME_MAX + PATH_MAX));
    char *input_path = (char *) malloc(sizeof(char) * (FILENAME_MAX + PATH_MAX));

    ubyte threshold;
    double scale;

    if (argc == 6) {
        SET_OR_DEFAULT(argv[1], input_path, DEFAULT_INPUT_PATH)
        SET_OR_DEFAULT(argv[2], input_filename, DEFAULT_INPUT_FILENAME)
        SET_OR_DEFAULT(argv[3], result_path, DEFAULT_RESULT_PATH)

        if (!IS_PNG(input_filename)) { ERROR_COUT_AND_RETURN(INVALID_FILE_TYPE) }

        // construct the input path
        strcat(input_path, input_filename);

        // check if the path is valid and the file exists
        if (!PATH_EXISTS(input_path)) { ERROR_COUT_AND_RETURN(INVALID_FILE_PATH) }

        // check if the result path is valid
        if (!PATH_EXISTS(result_path)) { ERROR_COUT_AND_RETURN(INVALID_RESULT_PATH) }

        // remove the .png extension
        input_filename[strlen(input_filename) - 4] = '\0';

        strcat(result_path, input_filename);
        strcat(result_path, "_sobel.png");

        // fourth arg is the threshold
        threshold = (ubyte) atoi(argv[4]);
        if (threshold < 0 || threshold > 255) { ERROR_COUT_AND_RETURN(INVALID_THRESHOLD) }

        // fifth arg is the scale
        scale = atof(argv[5]);
        if (scale < 0 || scale > 1) { ERROR_COUT_AND_RETURN(INVALID_SCALE_FACTOR) }

    } else if (argc == 1) {
        // use default values
        strcpy(input_path, DEFAULT_INPUT_PATH);
        strcpy(input_filename, DEFAULT_INPUT_FILENAME);
        strcat(input_path, input_filename);
        strcpy(result_path, DEFAULT_RESULT_PATH);
        threshold = SOBEL_THRESHOLD;
        scale = STRENGTH_RATIO;

        if (!IS_PNG(input_path)) { ERROR_COUT_AND_RETURN(INVALID_FILE_TYPE) }
        if (!PATH_EXISTS(input_path)) { ERROR_COUT_AND_RETURN(INVALID_FILE_PATH) }
        if (!PATH_EXISTS(result_path)) { ERROR_COUT_AND_RETURN(INVALID_RESULT_PATH) }
        if (threshold < 0 || threshold > 255) { ERROR_COUT_AND_RETURN(INVALID_THRESHOLD) }
        if (scale < 0 || scale > 1) { ERROR_COUT_AND_RETURN(INVALID_SCALE_FACTOR) }


        // remove the .png extension
        input_filename[strlen(input_filename) - 4] = '\0';
        strcat(result_path, input_filename);
        strcat(result_path, "_sobel.png");

        guide();

    } else {
        ERROR_COUT_AND_RETURN(INVALID_ARGUMENTS)
    }

    // read the image
    int width, height, bpp;
    ubyte *image = stbi_load(input_path, &width, &height, &bpp, 1);

    // start the timer
    auto start = std::chrono::high_resolution_clock::now();

    // apply the filters
    ubyte *edge_detected_image;
    detect_edges(
            image,
            &edge_detected_image,
            width, height,
            threshold, scale, 2);

    // stop the timer
    auto finish = std::chrono::high_resolution_clock::now();

    // write the image
    stbi_write_png(result_path, width, height, 1, edge_detected_image, width);


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
    stbi_image_free(edge_detected_image);

    return 0;
}
