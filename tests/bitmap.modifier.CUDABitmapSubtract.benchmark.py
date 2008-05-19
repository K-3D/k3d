#python

import testing
import k3d

# benchmark the CUDABitmapSubtract plugin for comparative purposes
testing.bitmap_benchmark("CUDABitmapSubtract", (640,480), 10)

testing.bitmap_benchmark("CUDABitmapSubtract", (800,600), 10)

testing.bitmap_benchmark("CUDABitmapSubtract", (1024,768), 10)

testing.bitmap_benchmark("CUDABitmapSubtract", (1280,1024), 10)
