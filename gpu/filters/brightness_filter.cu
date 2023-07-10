#include <iostream>
#include "brightness_filter.cuh"

__global__
void brightness_change_kernel(const ubyte *gray_scaled_img, ubyte *brightness_changed_img, size_t width, size_t height,
                              size_t channels,
                              int brightness_change) {
    // get the thread id
    size_t x = blockIdx.x * blockDim.x + threadIdx.x;
    size_t y = blockIdx.y * blockDim.y + threadIdx.y;

    // check if the thread is in the image
    if (x < width && y < height) {
        // get the index of the pixel
        size_t index = y * width + x;

        // change the brightness
        int val = gray_scaled_img[index] + brightness_change;
        val = val > UCHAR_MAX ? UCHAR_MAX : val < 0 ? 0 : val;

        // set the pixel
        brightness_changed_img[index] = val;
    }
}


int
change_brightness(const ubyte *gray_scaled_img, ubyte **brightness_changed_img, size_t width, size_t height,
                  size_t channels,
                  int brightness_change) {

    // Check if the input image and channels are valid
    if (gray_scaled_img == nullptr || channels != 1) {
        std::cout << "Invalid input image or number of channels. Expected a single-channel grayscale image.\n";
        return 1;
    }

    // allocate memory for the gray scale image
    *brightness_changed_img = (ubyte *) malloc(width * height * sizeof(ubyte));

    // check if the memory was allocated
    if (*brightness_changed_img == nullptr) {
        std::cout << "Failed to allocate memory for the gray scaled image!\n";
        return 1;
    }

    // allocate memory for the device image
    ubyte *device_gray_scaled_img;
    ubyte *device_brightness_changed_img;
    cudaMalloc(&device_gray_scaled_img, width * height * sizeof(ubyte));
    cudaMalloc(&device_brightness_changed_img, width * height * sizeof(ubyte));

    // copy the image to the device
    cudaMemcpy(device_gray_scaled_img, gray_scaled_img, width * height * sizeof(ubyte),
               cudaMemcpyHostToDevice);

    // calculate the number of blocks and threads
    dim3 block_size(32, 32);
    dim3 grid_size(width / block_size.x, height / block_size.y);
    if (width % block_size.x != 0) {
        grid_size.x++;
    }
    if (height % block_size.y != 0) {
        grid_size.y++;
    }

    // call the kernel
    brightness_change_kernel<<<grid_size, block_size>>>(device_gray_scaled_img,
                                                        device_brightness_changed_img, width,
                                                        height, channels, brightness_change);

    // copy the image from the device
    cudaMemcpy(*brightness_changed_img, device_brightness_changed_img, width * height * sizeof(ubyte),
               cudaMemcpyDeviceToHost);

    // free the memory
    cudaFree(device_gray_scaled_img);
    cudaFree(device_brightness_changed_img);

    return 0;
}
