nvcc main.cu ./filters/brightness_filter.cu ./filters/gray_scale_filter.cu ./filters/sobel_filter.cu -o prog.out

./prog.out

rm -rf prog.out
