#python

import testing
import k3d

# benchmark the CUDABitmapMultiply plugin for comparative purposes
testing.bitmap_benchmark("CUDABitmapMultiply", (640,480), 10)

testing.bitmap_benchmark("CUDABitmapMultiply", (800,600), 10)

testing.bitmap_benchmark("CUDABitmapMultiply", (1024,768), 10)

testing.bitmap_benchmark("CUDABitmapMultiply", (1280,1024), 10)

