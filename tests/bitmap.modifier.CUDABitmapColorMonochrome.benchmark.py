#python

import testing
import k3d

# benchmark the CUDABitmapColorMonochrome plugin for comparative purposes
testing.bitmap_benchmark("CUDABitmapColorMonochrome", (640,480), 10)

testing.bitmap_benchmark("CUDABitmapColorMonochrome", (800,600), 10)

testing.bitmap_benchmark("CUDABitmapColorMonochrome", (1024,768), 10)

testing.bitmap_benchmark("CUDABitmapColorMonochrome", (1280,1024), 10)


