# CUDA Task 1

## Build

- `nvcc -std=c++11 src/main.cu`  

## Run

```
Usage - command <kernel> [--b <input file> <output file> | --s <input file> <output file> [ <input file> <output file>] ]
Available kernels - box-blur edge-detect emboss gaussian-blur-3 gaussian-blur-5
```

Examples:

- `./a.out emboss --b input_img/big_3.png output_img/big_3.png`  
- `./main emboss --s input_img/small_4.png output_img/small_4.png input_img/small_5.png output_img/small_5.png input_img/small_6.png output_img/small_6.png`  

## Results

You can see kernels results in `/output_img`  
