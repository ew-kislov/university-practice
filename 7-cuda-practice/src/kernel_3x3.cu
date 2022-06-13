#include "config.cpp"

__global__ void apply_kernel_device_3x3(
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

    uint x = threadIdx.x + 1;
    uint y = threadIdx.y + 1;

    __shared__ unsigned char shared_image_r[BLOCK_DIM + 2][BLOCK_DIM + 2];
    __shared__ unsigned char shared_image_g[BLOCK_DIM + 2][BLOCK_DIM + 2];
    __shared__ unsigned char shared_image_b[BLOCK_DIM + 2][BLOCK_DIM + 2];

    shared_image_r[x][y] = input_image_r[linearY * width + linearX];
    shared_image_g[x][y] = input_image_g[linearY * width + linearX];
    shared_image_b[x][y] = input_image_b[linearY * width + linearX];

    bool isInner = (linearX > 0 && linearX < width - 1 && linearY > 0 && linearY < height - 1);

    if (!isInner) {
        output_image_r[linearY * width + linearX] = shared_image_r[x][y];
        output_image_g[linearY * width + linearX] = shared_image_g[x][y];
        output_image_b[linearY * width + linearX] = shared_image_b[x][y];

        return;
    }

    bool isLeftEdge = (threadIdx.x == 0);
    bool isRightEdge = (threadIdx.x == blockDim.x - 1);
    bool isTopEdge = (threadIdx.y == 0);
    bool isBottomEdge = (threadIdx.y == blockDim.y - 1);

    if (isLeftEdge) {
        shared_image_r[x - 1][y] = input_image_r[linearY * width + linearX - 1];
        shared_image_g[x - 1][y] = input_image_g[linearY * width + linearX - 1];
        shared_image_b[x - 1][y] = input_image_b[linearY * width + linearX - 1];
    }

    if (isRightEdge) {
        shared_image_r[x + 1][y] = input_image_r[linearY * width + linearX + 1];
        shared_image_g[x + 1][y] = input_image_g[linearY * width + linearX + 1];
        shared_image_b[x + 1][y] = input_image_b[linearY * width + linearX + 1];
    }

    if (isTopEdge) {
        shared_image_r[x][y - 1] = input_image_r[(linearY - 1) * width + linearX];
        shared_image_g[x][y - 1] = input_image_g[(linearY - 1) * width + linearX];
        shared_image_b[x][y - 1] = input_image_b[(linearY - 1) * width + linearX];
    }

    if (isBottomEdge) {
        shared_image_r[x][y + 1] = input_image_r[(linearY + 1) * width + linearX];
        shared_image_g[x][y + 1] = input_image_g[(linearY + 1) * width + linearX];
        shared_image_b[x][y + 1] = input_image_b[(linearY + 1) * width + linearX];
    }

    if (isTopEdge && isLeftEdge) {
        shared_image_r[x - 1][y - 1] = input_image_r[(linearY - 1) * width + linearX - 1];
        shared_image_g[x - 1][y - 1] = input_image_g[(linearY - 1) * width + linearX - 1];
        shared_image_b[x - 1][y - 1] = input_image_b[(linearY - 1) * width + linearX - 1];
    }

    if (isTopEdge && isRightEdge) {
        shared_image_r[x + 1][y - 1] = input_image_r[(linearY - 1) * width + linearX + 1];
        shared_image_g[x + 1][y - 1] = input_image_g[(linearY - 1) * width + linearX + 1];
        shared_image_b[x + 1][y - 1] = input_image_b[(linearY - 1) * width + linearX + 1];
    }

    if (isBottomEdge && isLeftEdge) {
        shared_image_r[x - 1][y + 1] = input_image_r[(linearY + 1) * width + linearX - 1];
        shared_image_g[x - 1][y + 1] = input_image_g[(linearY + 1) * width + linearX - 1];
        shared_image_b[x - 1][y + 1] = input_image_b[(linearY + 1) * width + linearX - 1];
    }

    if (isBottomEdge && isRightEdge) {
        shared_image_r[x + 1][y + 1] = input_image_r[(linearY + 1) * width + linearX + 1];
        shared_image_g[x + 1][y + 1] = input_image_g[(linearY + 1) * width + linearX + 1];
        shared_image_b[x + 1][y + 1] = input_image_b[(linearY + 1) * width + linearX + 1];
    }

    __syncthreads();

    float r = 0;
    float g = 0;
    float b = 0;

    // i = -1

    r += shared_image_r[x - 1][y - 1] * kernel[0];
    g += shared_image_g[x - 1][y - 1] * kernel[0];
    b += shared_image_b[x - 1][y - 1] * kernel[0];

    r += shared_image_r[x - 1][y] * kernel[1];
    g += shared_image_g[x - 1][y] * kernel[1];
    b += shared_image_b[x - 1][y] * kernel[1];

    r += shared_image_r[x - 1][y + 1] * kernel[2];
    g += shared_image_g[x - 1][y + 1] * kernel[2];
    b += shared_image_b[x - 1][y + 1] * kernel[2];

    // i = 0

    r += shared_image_r[x][y - 1] * kernel[3];
    g += shared_image_g[x][y - 1] * kernel[3];
    b += shared_image_b[x][y - 1] * kernel[3];

    r += shared_image_r[x][y] * kernel[4];
    g += shared_image_g[x][y] * kernel[4];
    b += shared_image_b[x][y] * kernel[4];

    r += shared_image_r[x][y + 1] * kernel[5];
    g += shared_image_g[x][y + 1] * kernel[5];
    b += shared_image_b[x][y + 1] * kernel[5];

    // i = 1

    r += shared_image_r[x + 1][y - 1] * kernel[6];
    g += shared_image_g[x + 1][y - 1] * kernel[6];
    b += shared_image_b[x + 1][y - 1] * kernel[6];

    r += shared_image_r[x + 1][y] * kernel[7];
    g += shared_image_g[x + 1][y] * kernel[7];
    b += shared_image_b[x + 1][y] * kernel[7];

    r += shared_image_r[x + 1][y + 1] * kernel[8];
    g += shared_image_g[x + 1][y + 1] * kernel[8];
    b += shared_image_b[x + 1][y + 1] * kernel[8];

    output_image_r[linearY * width + linearX] = ceil(r);
    output_image_g[linearY * width + linearX] = ceil(g);
    output_image_b[linearY * width + linearX] = ceil(b);
}