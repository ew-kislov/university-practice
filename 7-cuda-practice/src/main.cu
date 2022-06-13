#include "config.cpp"

#include "kernel_3x3.cu"
#include "kernel_5x5.cu"

struct CliOptions {
    string kernel;
    string input_file;
    string output_file;
    string input_dir;
    string output_dir;
};

const map<string, pair<char, float* > > kernels {
    {
        "box-blur",
        {
            3,
            new float[9] {
                1.0/9, 1.0/9, 1.0/9,
                1.0/9, 1.0/9, 1.0/9,
                1.0/9, 1.0/9, 1.0/9
            }
        }
    },
    {
        "gaussian-blur-3",
        {
            3,
            new float[9] {
                1.0/16, 2.0/16, 1.0/16,
                2.0/16, 4.0/16, 2.0/16,
                1.0/16, 2.0/16, 1.0/16
            }
        }
    },
    {
        "gaussian-blur-5",
        {
            5,
            new float[25] {
                1.0/256, 4.0/256,   6.0/256,  4.0/256, 1.0/256,
                4.0/256, 16.0/256, 24.0/256, 16.0/256, 4.0/256,
                6.0/256, 24.0/256, 36.0/256, 24.0/256, 6.0/256,
                4.0/256, 16.0/256, 24.0/256, 16.0/256, 4.0/256,
                1.0/256, 4.0/256,   6.0/256,  4.0/256, 1.0/256
            }
        }
    },
    {
        "edge-detect",
        {
            3,
            new float[9] {
                -1, -1, -1,
                -1,  8, -1,
                -1, -1, -1
            }
        }
    },
    {
        "emboss",
        {
            3,
            new float[9] {
                -2, -1, 0,
                -1,  1, 1,
                 0,  1, 2
            }
        }
    }
};

int maxBlockSize;
int maxBlockDimX;
int maxBlockDimY;
int maxGridDimX;
int maxGridDimY;

vector<string> get_files_from_dir(string name) {
    DIR* dir;
    struct dirent* entry;
    vector<string> files;

    if (!(dir = opendir(name.c_str())))
        return files;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            files.push_back(entry->d_name);
        }
    }
    closedir(dir);

    return files;
}

void getError(cudaError_t err) {
    if (err != cudaSuccess) {
        printf("CUDA error - %s\n", cudaGetErrorString(err));
    }
}

pair<float, float> apply_kernel(unsigned char* input_image, unsigned char* output_image, int width, int height, string kernel_type) {
    unsigned char* host_input_r = new unsigned char[width * height];
    unsigned char* host_input_g = new unsigned char[width * height];
    unsigned char* host_input_b = new unsigned char[width * height];
    unsigned char* host_output_r = new unsigned char[width * height];
    unsigned char* host_output_g = new unsigned char[width * height];
    unsigned char* host_output_b = new unsigned char[width * height];
    
    unsigned char* dev_input_r;
    unsigned char* dev_input_g;
    unsigned char* dev_input_b;
    unsigned char* dev_output_r;
    unsigned char* dev_output_g;
    unsigned char* dev_output_b;

    for (int i = 0; i < 3 * width * height; i += 3) {
        host_input_r[i / 3] = input_image[i];
        host_input_g[i / 3] = input_image[i + 1];
        host_input_b[i / 3] = input_image[i + 2];
    }

    cudaStream_t stream;
    cudaStreamCreate(&stream);

    float* dev_kernel;

    float ms_outer = 0;
    float ms_inner = 0;
    cudaEvent_t start_outer;
    cudaEvent_t stop_outer;
    cudaEvent_t start_inner;
    cudaEvent_t stop_inner;
    cudaEventCreate(&start_outer);
    cudaEventCreate(&stop_outer);
    cudaEventCreate(&start_inner);
    cudaEventCreate(&stop_inner);

    cudaEventRecord(start_outer);
    cudaEventSynchronize(start_outer);

    getError(cudaMalloc((void **)&dev_kernel, sqr(kernels.at(kernel_type).first) * sizeof(float)));
    getError(cudaMalloc((void **)&dev_input_r, width * height * sizeof(unsigned char)));
    getError(cudaMalloc((void **)&dev_input_b, width * height * sizeof(unsigned char)));
    getError(cudaMalloc((void **)&dev_input_g, width * height * sizeof(unsigned char)));
    getError(cudaMalloc((void **)&dev_output_r, width * height * sizeof(unsigned char)));
    getError(cudaMalloc((void **)&dev_output_g, width * height * sizeof(unsigned char)));
    getError(cudaMalloc((void **)&dev_output_b, width * height * sizeof(unsigned char)));

    getError(cudaMemcpyAsync(dev_kernel, kernels.at(kernel_type).second, sqr(kernels.at(kernel_type).first) * sizeof(float), cudaMemcpyHostToDevice, stream));
    getError(cudaMemcpyAsync(dev_input_r, host_input_r, width * height * sizeof(unsigned char), cudaMemcpyHostToDevice, stream));
    getError(cudaMemcpyAsync(dev_input_g, host_input_g, width * height * sizeof(unsigned char), cudaMemcpyHostToDevice, stream));
    getError(cudaMemcpyAsync(dev_input_b, host_input_b, width * height * sizeof(unsigned char), cudaMemcpyHostToDevice, stream));

    int blockDim = BLOCK_DIM;
    int gridDimX = ceil(1.0 * width / blockDim);
    int gridDimY = ceil(1.0 * height / blockDim);

    if (gridDimX > maxGridDimX || gridDimY > maxGridDimY) {
        throw runtime_error("Too big image");
    }

    printf("Device params: block size %d, grid x-dim %d, grid y-dim %d\n", blockDim, gridDimX, gridDimY);

    dim3 blockDims(blockDim, blockDim, 1);
    dim3 gridDims(gridDimX, gridDimY, 1);

    cudaEventRecord(start_inner);
    cudaEventSynchronize(start_inner);

    if (kernels.at(kernel_type).first == 3) {
        apply_kernel_device_3x3<<<gridDims, blockDims, 0, stream>>>(
            dev_input_r, dev_input_g, dev_input_b,
            dev_output_r, dev_output_g, dev_output_b,
            width, height,
            dev_kernel
        );
    } else if (kernels.at(kernel_type).first == 5) {
        apply_kernel_device_5x5<<<gridDims, blockDims, 0, stream>>>(
            dev_input_r, dev_input_g, dev_input_b,
            dev_output_r, dev_output_g, dev_output_b,
            width, height,
            dev_kernel
        );
    }

    cudaEventRecord(stop_inner);
    cudaEventSynchronize(stop_inner);
    cudaEventElapsedTime(&ms_inner, start_inner, stop_inner);

    getError(cudaMemcpyAsync(host_output_r, dev_output_r, width * height * sizeof(unsigned char), cudaMemcpyDeviceToHost, stream));
    getError(cudaMemcpyAsync(host_output_g, dev_output_g, width * height * sizeof(unsigned char), cudaMemcpyDeviceToHost, stream));
    getError(cudaMemcpyAsync(host_output_b, dev_output_b, width * height * sizeof(unsigned char), cudaMemcpyDeviceToHost, stream));

    getError(cudaFree(dev_input_r));
    getError(cudaFree(dev_input_g));
    getError(cudaFree(dev_input_b));
    getError(cudaFree(dev_output_r));
    getError(cudaFree(dev_output_g));
    getError(cudaFree(dev_output_b));
    getError(cudaFree(dev_kernel));

    cudaEventRecord(stop_outer);
    cudaEventSynchronize(stop_outer);
    cudaEventElapsedTime(&ms_outer, start_outer, stop_outer);

    cudaStreamDestroy(stream);

    printf("GPU calculation time: %g ms\n", ms_inner);
    printf("GPU calculation + transport time: %g ms\n", ms_outer);

    for (int i = 0; i < 3 * width * height; i += 3) {
        output_image[i] = host_output_r[i / 3];
        output_image[i + 1] = host_output_g[i / 3];
        output_image[i + 2] = host_output_b[i / 3];
    }

    return make_pair(ms_inner, ms_outer);
}

void loadCudaSettings() {
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);

    maxBlockSize = prop.maxThreadsPerBlock;
    maxBlockDimX = prop.maxThreadsDim[0];
    maxBlockDimY = prop.maxThreadsDim[1];
    maxGridDimX = prop.maxGridSize[0];
    maxGridDimY = prop.maxGridSize[1];

    printf("CUDA block max size - %d\n", prop.maxThreadsPerBlock);
    printf("CUDA block max dimensions - %d, %d, %d\n", prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2]);
    printf("CUDA grid max dimensions - %d, %d, %d\n", prop.maxGridSize[0], prop.maxGridSize[1], prop.maxGridSize[2]);
}

CliOptions parseCli(int argc, char** argv) {
    enum CliCommandState {
        KernelType,
        ImageType,
        BigImageParse,
        SmallImagesParseItems,
        Done,
        Error
    };

    CliCommandState state = KernelType;

    CliOptions options;

    int i = 1;
    while (state != Done) {
        if (i > argc - 1) {
            state = Error;    
        }
        switch (state) {
            case KernelType: {
                bool found = false;
                for (auto const entry: kernels) {
                    if (entry.first == argv[i]) {
                        found = true;
                        options.kernel = entry.first;
                    }
                }
                if (!found) {
                    printf("Error: kernel with given name doesn't exist\n");
                    state = Error;
                }
                state = ImageType;
                i++;
                break;
            }
            case ImageType: {
                if (strcmp(argv[i], "--b") == 0) {
                    state = BigImageParse;
                } else if (strcmp(argv[i], "--s") == 0) {
                    state = SmallImagesParseItems;
                } else {
                    printf("Error: expected flag --b or --s\n");
                    state = Error;
                }
                i++;
                break;
            }
            case BigImageParse: {
                options.input_file = argv[i];
                options.output_file = argv[i + 1];
                i += 2;
                state = Done;
                break;
            }
            case SmallImagesParseItems: {
                options.input_dir = argv[i];
                options.output_dir = argv[i + 1];
                i += 2;
                state = Done;
                break;
            }
            case Error:
                printf("Usage - command <kernel> [--b <input file> <output file> | --s <input dir> <output dir> ]\n");
                printf("Available kernels - ");
                for (auto const entry: kernels) {
                    printf("%s ", entry.first.c_str());
                }
                printf("\n");
                exit(1);
            default:
                throw runtime_error("Unknown cli state");
        }
    }

    return options;
}

pair<float, float> processImage(const char* input_file, const char* output_file, string kernel_type) {
    vector<unsigned char> input_image_raw;
    unsigned int width, height;

    unsigned error = lodepng::decode(input_image_raw, width, height, input_file);
    if (error) {
        cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;
    }

    unsigned char *input_image = new unsigned char[(input_image_raw.size() * 3) / 4];
    unsigned char *output_image = new unsigned char[(input_image_raw.size() * 3) / 4];
    int inp_iterator = 0;
    for (int i = 0; i < input_image_raw.size(); ++i) {
        if ((i + 1) % 4 != 0) {
            input_image[inp_iterator] = input_image_raw.at(i);
            output_image[inp_iterator] = 255;
            inp_iterator++;
        }
    }

    printf("Image size - %dx%d\n", width, height);

    pair<float, float> time_recording = apply_kernel(input_image, output_image, width, height, kernel_type);

    cudaDeviceSynchronize();

    int out_iterator = 0;
    vector<unsigned char> output_image_raw(input_image_raw.size());
    for (int i = 0; i < width * height * 3; ++i) {
        output_image_raw[out_iterator] = output_image[i];
        out_iterator++;
        if ((i + 1) % 3 == 0) {
            output_image_raw[out_iterator] = 255;
            out_iterator++;
        }
    }

    error = lodepng::encode(output_file, output_image_raw, width, height);

    if (error) {
        printf("Encoder error: %s\n", lodepng_error_text(error));
    }

    delete[] input_image;
    delete[] output_image;

    return time_recording;
}

int main(int argc, char** argv) {
    CliOptions options = parseCli(argc, argv);

    loadCudaSettings();

    float total_outer_time = 0;
    float total_inner_time = 0;

    vector<pair<string, string> > images;

    if (options.input_dir != "") {
        for (auto entry : get_files_from_dir(options.input_dir)) {
            cout << entry << endl;
            images.push_back(make_pair(options.input_dir + "/" + entry, options.output_dir + "/" + entry));
        }
    } else {
        images.push_back(make_pair(options.input_file, options.output_file));
    }

    for (int i = 0; i < images.size(); i++) {
        printf("Started processing image %s\n", images[i].first.c_str());

        pair<float, float> time_recording = processImage(images[i].first.c_str(), images[i].second.c_str(), options.kernel);
        total_inner_time += time_recording.first;
        total_outer_time += time_recording.second;

        printf("Finised. Output was written to %s\n", images[i].second.c_str());
    }

    printf("Total time: calculations - %g, calculations + transport - %g\n", total_inner_time, total_outer_time);
    
    return 0;
}