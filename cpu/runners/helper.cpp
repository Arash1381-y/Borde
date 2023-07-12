
#define SET_OR_DEFAULT(input, var, default) \
    if (strcmp(input, "-") == 0) { \
        strcpy(var, default); \
    } else { \
        strcpy(var, input); \
    }



// decleare some error types messages
#define INVALID_ARGUMENTS "Invalid number of arguments!"
#define INVALID_FILE_TYPE "Invalid file type! Only png files are supported!"
#define INVALID_FILE_PATH "Invalid file path!"
#define INVALID_RESULT_PATH "Invalid result path!"
#define INVALID_BRIGHTNESS_CHANGE "Invalid brightness change value! It should be between -128 and 127!"
#define INVALID_SCALE_FACTOR "Invalid scale factor value! It should be between 0 and 1!"
#define INVALID_THRESHOLD "Invalid threshold value! It should be between 0 and 255!"

// macro for checking if the file is PNG or not
#define IS_PNG(filename) (strstr(filename, ".png") != nullptr)

// ERROR COUT AND RETURN
#define ERROR_COUT_AND_RETURN(message) \
    std::cout  << "\033[1;33m" << message << "\n" << "\033[0m"; \
    return 1;

// FILE EXISTS
# define PATH_EXISTS(path) fs::exists(path)

