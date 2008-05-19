#python

import testing
import k3d

# benchmark the CUDABitmapSubtract plugin as well as the BitmapSubtract plugin for comparative purposes
testing.bitmap_benchmark("CUDABitmapSubtract", (640,480), 10)
testing.bitmap_benchmark("BitmapSubtract", (640,480), 10)

testing.bitmap_benchmark("CUDABitmapSubtract", (800,600), 10)
testing.bitmap_benchmark("BitmapSubtract", (800,600), 10)

testing.bitmap_benchmark("CUDABitmapSubtract", (1024,768), 10)
testing.bitmap_benchmark("BitmapSubtract", (1024,768), 10)

testing.bitmap_benchmark("CUDABitmapSubtract", (1280,1024), 10)
testing.bitmap_benchmark("BitmapSubtract", (1280,1024), 10)


