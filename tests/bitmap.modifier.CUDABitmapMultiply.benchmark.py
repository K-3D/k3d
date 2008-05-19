#python

import testing
import k3d

# benchmark the CUDABitmapMultiply plugin as well as the BitmapMultiply plugin for comparative purposes
testing.bitmap_benchmark("CUDABitmapMultiply", (640,480), 10)
testing.bitmap_benchmark("BitmapMultiply", (640,480), 10)

testing.bitmap_benchmark("CUDABitmapMultiply", (800,600), 10)
testing.bitmap_benchmark("BitmapMultiply", (800,600), 10)

testing.bitmap_benchmark("CUDABitmapMultiply", (1024,768), 10)
testing.bitmap_benchmark("BitmapMultiply", (1024,768), 10)

testing.bitmap_benchmark("CUDABitmapMultiply", (1280,1024), 10)
testing.bitmap_benchmark("BitmapMultiply", (1280,1024), 10)


