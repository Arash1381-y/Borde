
#include <iostream>
#include <chrono>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../stb/stb_image.h"

#include "../stb/stb_image_write.h"

#include "../filters//sobel_filter.h"

#include "../../config.h"

namespace fs = std::filesystem;


int main(int argc, char *argv[]) {

    char *input_filename = (char *) malloc(sizeof(char) * FILENAME_MAX);
    char *result_path = (char *) malloc(sizeof(char) * (FILENAME_MAX + PATH_MAX));
    char *input_path = (char *) malloc(sizeof(char) * (FILENAME_MAX + PATH_MAX));

    ubyte threshold = SOBEL_THRESHOLD;
    double scale = STRENGTH_RATIO;

    if (argc == 4) {
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
        strcat(result_path, "_sobel.png");

        // read the threshold and scale
        threshold = (ubyte) atoi(argv[2]);
        scale = (double) atof(argv[3]);

        // check if the threshold is valid
        if (threshold < 0 || threshold > 255) {
            // print threshold


            std::cout << threshold << " __ Invalid threshold value! It should be between 0 and 255!\n";
            return 1;
        }

        // check if the scale is valid
        if (scale < 0 || scale > 1) {
            std::cout << "Invalid scale value! It should be between 0 and 1!\n";
            return 1;
        }


    } else {
        std::cout << "\033[1;31m" << "No arguments provided! Using default setup!\n" << "\033[0m";

        // GUIDE
        std::cout << "\033[1;31m" << "Usage : ./sobel_runner <filename> <threshold> <scale>\n" << "\033[0m";

        // DEFAULTS
        std::cout << "\033[1;31m" << "Default filename : " << "input_sobel.png" << "\n" << "\033[0m";
        std::cout << "\033[1;31m" << "Default threshold : " << (int) SOBEL_THRESHOLD << "\n" << "\033[0m";
        std::cout << "\033[1;31m" << "Default scale : " << STRENGTH_RATIO << "\n" << "\033[0m";

        // EXAMPLE
        std::cout << "\033[1;31m" << "Example : ./sobel_runner input.png 100 0.5\n" << "\033[0m";
        std::cout << "\033[1;31m" << "----------------------------------------\n" << "\033[0m";


        strcpy(input_path, INPUT_PATH);
        strcat(input_path, "input_sobel.png");

        // print the path in yellow
        std::cout << "\033[1;33m" << "Input image : " << input_path << "\033[0m\n";

        // check if the path is valid and the file exists
        if (!fs::exists(input_path)) {
            std::cout << "Invalid file path!\n";
            return 1;
        }

        strcpy(result_path, RESULT_PATH);
        strcat(result_path, "input_sobel.png");
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
    auto stop = std::chrono::high_resolution_clock::now();

    // write the image
    stbi_write_png(result_path, width, height, 1, edge_detected_image, width);


    // print the time and the result path
    std::cout << "----------------------------------------\n";
    std::cout << "Edge detection took : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()
              << " milliseconds\n";
    std::cout << "----------------------------------------\n";

    std::cout << "\033[1;32m" << "Result saved in : " << result_path << "\033[0m\n";
    std::cout << "\033[1;32m" << "----------------------------------------\n" << "\033[0m";

    // free the memory
    free(input_filename);
    free(result_path);
    free(input_path);
    stbi_image_free(image);
    stbi_image_free(edge_detected_image);

    return 0;
}
