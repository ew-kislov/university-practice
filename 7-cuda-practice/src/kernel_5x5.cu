#include "config.cpp"

__global__ void apply_kernel_device_5x5(
    unsigned char* input_image_r,
    unsigned char* input_image_g,
    unsigned char* input_image_b,
    unsigned char* output_image_r,
    unsigned char* output_image_g,
    unsigned char* output_image_b,
    int width,
    int height,
    float* kernel
) {
    const unsigned int linearX = blockIdx.x * blockDim.x + threadIdx.x;
    const unsigned int linearY = blockIdx.y * blockDim.y + threadIdx.y;

    if (linearX >= width || linearY >= height) {
        return;
    }

    bool isInner = (linearX > 1 && linearX < width - 2 && linearY > 2 && linearY < height - 2);

    if (!isInner) {
        output_image_r[linearY * width + linearX] = input_image_r[linearY * width + linearX];
        output_image_g[linearY * width + linearX] = input_image_g[linearY * width + linearX];
        output_image_b[linearY * width + linearX] = input_image_b[linearY * width + linearX];

        return;
    }

    float r = 0;
    float g = 0;
    float b = 0;

    // i = -2
    
    r += input_image_r[(linearY - 2) * width + (linearX - 2)] * kernel[0];
    g += input_image_g[(linearY - 2) * width + (linearX - 2)] * kernel[0];
    b += input_image_b[(linearY - 2) * width + (linearX - 2)] * kernel[0];

    r += input_image_r[(linearY - 2) * width + (linearX - 1)] * kernel[1];
    g += input_image_g[(linearY - 2) * width + (linearX - 1)] * kernel[1];
    b += input_image_b[(linearY - 2) * width + (linearX - 1)] * kernel[1];

    r += input_image_r[(linearY - 2) * width + (linearX)] * kernel[2];
    g += input_image_g[(linearY - 2) * width + (linearX)] * kernel[2];
    b += input_image_b[(linearY - 2) * width + (linearX)] * kernel[2];

    r += input_image_r[(linearY - 2) * width + (linearX + 1)] * kernel[3];
    g += input_image_g[(linearY - 2) * width + (linearX + 1)] * kernel[3];
    b += input_image_b[(linearY - 2) * width + (linearX + 1)] * kernel[3];

    r += input_image_r[(linearY - 2) * width + (linearX + 2)] * kernel[4];
    g += input_image_g[(linearY - 2) * width + (linearX + 2)] * kernel[4];
    b += input_image_b[(linearY - 2) * width + (linearX + 2)] * kernel[4];

    // i = -1
    
    r += input_image_r[(linearY - 1) * width + (linearX - 2)] * kernel[5];
    g += input_image_g[(linearY - 1) * width + (linearX - 2)] * kernel[5];
    b += input_image_b[(linearY - 1) * width + (linearX - 2)] * kernel[5];

    r += input_image_r[(linearY - 1) * width + (linearX - 1)] * kernel[6];
    g += input_image_g[(linearY - 1) * width + (linearX - 1)] * kernel[6];
    b += input_image_b[(linearY - 1) * width + (linearX - 1)] * kernel[6];

    r += input_image_r[(linearY - 1) * width + (linearX)] * kernel[7];
    g += input_image_g[(linearY - 1) * width + (linearX)] * kernel[7];
    b += input_image_b[(linearY - 1) * width + (linearX)] * kernel[7];

    r += input_image_r[(linearY - 1) * width + (linearX + 1)] * kernel[8];
    g += input_image_g[(linearY - 1) * width + (linearX + 1)] * kernel[8];
    b += input_image_b[(linearY - 1) * width + (linearX + 1)] * kernel[8];

    r += input_image_r[(linearY - 1) * width + (linearX + 2)] * kernel[9];
    g += input_image_g[(linearY - 1) * width + (linearX + 2)] * kernel[9];
    b += input_image_b[(linearY - 1) * width + (linearX + 2)] * kernel[9];

    // i = 0
    
    r += input_image_r[(linearY) * width + (linearX - 2)] * kernel[10];
    g += input_image_g[(linearY) * width + (linearX - 2)] * kernel[10];
    b += input_image_b[(linearY) * width + (linearX - 2)] * kernel[10];

    r += input_image_r[(linearY) * width + (linearX - 1)] * kernel[11];
    g += input_image_g[(linearY) * width + (linearX - 1)] * kernel[11];
    b += input_image_b[(linearY) * width + (linearX - 1)] * kernel[11];

    r += input_image_r[(linearY) * width + (linearX)] * kernel[12];
    g += input_image_g[(linearY) * width + (linearX)] * kernel[12];
    b += input_image_b[(linearY) * width + (linearX)] * kernel[12];

    r += input_image_r[(linearY) * width + (linearX + 1)] * kernel[13];
    g += input_image_g[(linearY) * width + (linearX + 1)] * kernel[13];
    b += input_image_b[(linearY) * width + (linearX + 1)] * kernel[13];

    r += input_image_r[(linearY) * width + (linearX + 2)] * kernel[14];
    g += input_image_g[(linearY) * width + (linearX + 2)] * kernel[14];
    b += input_image_b[(linearY) * width + (linearX + 2)] * kernel[14];

    // i = 1
    
    r += input_image_r[(linearY + 1) * width + (linearX - 2)] * kernel[15];
    g += input_image_g[(linearY + 1) * width + (linearX - 2)] * kernel[15];
    b += input_image_b[(linearY + 1) * width + (linearX - 2)] * kernel[15];

    r += input_image_r[(linearY + 1) * width + (linearX - 1)] * kernel[16];
    g += input_image_g[(linearY + 1) * width + (linearX - 1)] * kernel[16];
    b += input_image_b[(linearY + 1) * width + (linearX - 1)] * kernel[16];

    r += input_image_r[(linearY + 1) * width + (linearX)] * kernel[17];
    g += input_image_g[(linearY + 1) * width + (linearX)] * kernel[17];
    b += input_image_b[(linearY + 1) * width + (linearX)] * kernel[17];

    r += input_image_r[(linearY + 1) * width + (linearX + 1)] * kernel[18];
    g += input_image_g[(linearY + 1) * width + (linearX + 1)] * kernel[18];
    b += input_image_b[(linearY + 1) * width + (linearX + 1)] * kernel[18];

    r += input_image_r[(linearY + 1) * width + (linearX + 2)] * kernel[19];
    g += input_image_g[(linearY + 1) * width + (linearX + 2)] * kernel[19];
    b += input_image_b[(linearY + 1) * width + (linearX + 2)] * kernel[19];

    // i = 2
    
    r += input_image_r[(linearY + 2) * width + (linearX - 2)] * kernel[20];
    g += input_image_g[(linearY + 2) * width + (linearX - 2)] * kernel[20];
    b += input_image_b[(linearY + 2) * width + (linearX - 2)] * kernel[20];

    r += input_image_r[(linearY + 2) * width + (linearX - 1)] * kernel[21];
    g += input_image_g[(linearY + 2) * width + (linearX - 1)] * kernel[21];
    b += input_image_b[(linearY + 2) * width + (linearX - 1)] * kernel[21];

    r += input_image_r[(linearY + 2) * width + (linearX)] * kernel[22];
    g += input_image_g[(linearY + 2) * width + (linearX)] * kernel[22];
    b += input_image_b[(linearY + 2) * width + (linearX)] * kernel[22];

    r += input_image_r[(linearY + 2) * width + (linearX + 1)] * kernel[23];
    g += input_image_g[(linearY + 2) * width + (linearX + 1)] * kernel[23];
    b += input_image_b[(linearY + 2) * width + (linearX + 1)] * kernel[23];

    r += input_image_r[(linearY + 2) * width + (linearX + 2)] * kernel[24];
    g += input_image_g[(linearY + 2) * width + (linearX + 2)] * kernel[24];
    b += input_image_b[(linearY + 2) * width + (linearX + 2)] * kernel[24];

    output_image_r[linearY * width + linearX] = ceil(r);
    output_image_g[linearY * width + linearX] = ceil(g);
    output_image_b[linearY * width + linearX] = ceil(b);
}