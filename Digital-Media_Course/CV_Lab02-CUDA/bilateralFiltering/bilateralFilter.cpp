/**
 * @author: poodarchu
 * @date: 2016-11-06
 *
 * Image bilateral filtering
 * 双边滤波是一种保边缘、非线性平滑滤波器，这种滤波器有 3 个参数：gaussian delta, euclidean delta and iterations
 * 随着 euclidean delta 值增大，大多数纹理信息会被过滤掉，然后轮廓信息则会被保留。当 euclidean delta 接近无穷大时，filter 就变成了一个
 * 普通的高斯滤波器。
 * 随着 gaussian delta 增大，图像细致的纹理会变得越来越模糊。
 * 多次迭代有将一幅图片扁平化的效果，这并不会模糊边缘，而是产生一种卡通画的效果。
 */

#include <math.h>

// CUDA utilities and system includes
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include <helper_cuda.h>   // CUDA device initialization helper functions
#include <helper_cuda_gl.h> //CUDA device + OpenGL initialization functions

// Shared Library Test functions
#include <helper_functions.h>   //CUDA SDK helper functions


const char *image_filename = "nature_monte.bmp";
int iterations = 1;
float gaussian_delta = 4;
float euclidean_delta = 0.1f;
int filter_radius = 5;

unsigned int width, height;
unsigned int  *hImage  = NULL;

extern "C" void loadImageData(int argc, char **argv);

// These are CUDA functions to handle allocation and launching the kernels
extern "C" void initTexture(int width, int height, void *pImage);
extern "C" void freeTextures();
extern "C" double bilateralFilterRGBA(unsigned int *d_dest, int width, int height, float e_d, int radius, int iterations, StopWatchInterface *timer);
extern "C" void updateGaussian(float delta, int radius);
extern "C" void updateGaussianGold(float delta, int radius);
extern "C" void bilateralFilterGold(unsigned int *pSrc, unsigned int *pDest, float e_d, int w, int h, int r);
extern "C" void LoadBMPFile(uchar4 **dst, unsigned int *width, unsigned int *height, const char *name);


void initCuda()
{
    //initialize gaussian mask
    updateGaussian(gaussian_delta, filter_radius);

    initTexture(width, height, hImage);
    sdkCreateTimer(&timer);
    sdkCreateTimer(&kernel_timer);
}

void cleanup()
{
    sdkDeleteTimer(&timer);
    sdkDeleteTimer(&kernel_timer);

    if (hImage)
    {
        free(hImage);
    }

    freeTextures();

    //DEPRECATED: checkCudaErrors(cudaGLUnregisterBufferObject(pbo));
    cudaGraphicsUnregisterResource(cuda_pbo_resource);

    // cudaDeviceReset causes the driver to clean up all state. While
    // not mandatory in normal operation, it is good practice.  It is also
    // needed to ensure correct operation when the application is being
    // profiled. Calling cudaDeviceReset causes all profile data to be
    // flushed before the application exits
    cudaDeviceReset();
}

void initGLResources()
{
    // create pixel buffer object
    glGenBuffersARB(1, &pbo);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, width*height*sizeof(GLubyte)*4, hImage, GL_STREAM_DRAW_ARB);

    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    // DEPRECATED: checkCudaErrors(cudaGLRegisterBufferObject(pbo));
    checkCudaErrors(cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo,
                                                 cudaGraphicsMapFlagsWriteDiscard));

    // create texture for display
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // load shader program
    shader = compileASMShader(GL_FRAGMENT_PROGRAM_ARB, shader_code);
}

////////////////////////////////////////////////////////////////////////////////
//! Run a simple benchmark test for CUDA
////////////////////////////////////////////////////////////////////////////////
int runBenchmark(int argc, char **argv)
{
    printf("[runBenchmark]: [%s]\n", sSDKsample);

    loadImageData(argc, argv);
    initCuda();

    unsigned int *dResult;
    size_t pitch;
    checkCudaErrors(cudaMallocPitch((void **)&dResult, &pitch, width*sizeof(unsigned int), height));
    sdkStartTimer(&kernel_timer);

    // warm-up
    bilateralFilterRGBA(dResult, width, height, euclidean_delta, filter_radius, iterations, kernel_timer);
    checkCudaErrors(cudaDeviceSynchronize());

    // Start round-trip timer and process iCycles loops on the GPU
    iterations = 1;     // standard 1-pass filtering
    const int iCycles = 150;
    double dProcessingTime = 0.0;
    printf("\nRunning BilateralFilterGPU for %d cycles...\n\n", iCycles);

    for (int i = 0; i < iCycles; i++)
    {
        dProcessingTime += bilateralFilterRGBA(dResult, width, height, euclidean_delta, filter_radius, iterations, kernel_timer);
    }

    // check if kernel execution generated an error and sync host
    getLastCudaError("Error: bilateralFilterRGBA Kernel execution FAILED");
    checkCudaErrors(cudaDeviceSynchronize());
    sdkStopTimer(&kernel_timer);

    // Get average computation time
    dProcessingTime /= (double)iCycles;

    // log testname, throughput, timing and config info to sample and master logs
    printf("bilateralFilter-texture, Throughput = %.4f M RGBA Pixels/s, Time = %.5f s, Size = %u RGBA Pixels, NumDevsUsed = %u\n",
           (1.0e-6 * width * height)/dProcessingTime, dProcessingTime, (width * height), 1);
    printf("\n");

    return 0;
}

// This test specifies a single test (where you specify radius and/or iterations)
int runSingleTest(char *ref_file, char *exec_path)
{
    int nTotalErrors = 0;
    char dump_file[256];

    printf("[runSingleTest]: [%s]\n", sSDKsample);

    initCuda();

    unsigned int *dResult;
    unsigned int *hResult = (unsigned int *)malloc(width * height * sizeof(unsigned int));
    size_t pitch;
    checkCudaErrors(cudaMallocPitch((void **)&dResult, &pitch, width*sizeof(unsigned int), height));

    // run the sample radius
    {
        printf("%s (radius=%d) (passes=%d) ", sSDKsample, filter_radius, iterations);
        bilateralFilterRGBA(dResult, width, height, euclidean_delta, filter_radius, iterations, kernel_timer);

        // check if kernel execution generated an error
        getLastCudaError("Error: bilateralFilterRGBA Kernel execution FAILED");
        checkCudaErrors(cudaDeviceSynchronize());

        // readback the results to system memory
        cudaMemcpy2D(hResult, sizeof(unsigned int)*width, dResult, pitch,
                     sizeof(unsigned int)*width, height, cudaMemcpyDeviceToHost);

        sprintf(dump_file, "nature_%02d.ppm", filter_radius);

        sdkSavePPM4ub((const char *)dump_file, (unsigned char *)hResult, width, height);

        if (!sdkComparePPM(dump_file, sdkFindFilePath(ref_file, exec_path), MAX_EPSILON_ERROR, 0.15f, false))
        {
            printf("Image is Different ");
            nTotalErrors++;
        }
        else
        {
            printf("Image is Matching ");
        }

        printf(" <%s>\n", ref_file);
    }
    printf("\n");

    free(hResult);
    checkCudaErrors(cudaFree(dResult));

    return nTotalErrors;
}

void loadImageData(int argc, char **argv)
{
    // load image (needed so we can get the width and height before we create the window
    char *image_path = NULL;

    if (argc >= 1)
    {
        image_path = sdkFindFilePath(image_filename, argv[0]);
    }

    if (image_path == NULL)
    {
        fprintf(stderr, "Error finding image file '%s'\n", image_filename);
        exit(EXIT_FAILURE);
    }

    LoadBMPFile((uchar4 **)&hImage, &width, &height, image_path);

    if (!hImage)
    {
        fprintf(stderr, "Error opening file '%s'\n", image_path);
        exit(EXIT_FAILURE);
    }

    printf("Loaded '%s', %d x %d pixels\n\n", image_path, width, height);
}


int main(int argc, char** argv) {
  loadImageData(argc, argv);

  initCuda();

  
}
