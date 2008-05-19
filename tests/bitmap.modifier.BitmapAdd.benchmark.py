#python

import testing
import k3d

# benchmark the BitmapAdd plugin for comparative purposes
testing.bitmap_benchmark("BitmapAdd", (640,480), 10)

testing.bitmap_benchmark("BitmapAdd", (800,600), 10)

testing.bitmap_benchmark("BitmapAdd", (1024,768), 10)

testing.bitmap_benchmark("BitmapAdd", (1280,1024), 10)


