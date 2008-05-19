#python

import testing
import k3d

# benchmark the BitmapSubtract plugin for comparative purposes
testing.bitmap_benchmark("BitmapSubtract", (640,480), 10)

testing.bitmap_benchmark("BitmapSubtract", (800,600), 10)

testing.bitmap_benchmark("BitmapSubtract", (1024,768), 10)

testing.bitmap_benchmark("BitmapSubtract", (1280,1024), 10)


