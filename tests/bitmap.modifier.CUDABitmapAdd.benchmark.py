#python

import testing
import k3d

# benchmark the CUDABitmapAdd plugin for comparative purposes
testing.bitmap_benchmark("CUDABitmapAdd", (640,480), 10)

testing.bitmap_benchmark("CUDABitmapAdd", (800,600), 10)

testing.bitmap_benchmark("CUDABitmapAdd", (1024,768), 10)

testing.bitmap_benchmark("CUDABitmapAdd", (1280,1024), 10)