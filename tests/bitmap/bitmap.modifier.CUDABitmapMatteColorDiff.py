#python
import testing

testing.bitmap_compare_plugin_outputs("BitmapMatteColorDiff", "CUDABitmapMatteColorDiff", {"value": 0.0 })
testing.bitmap_compare_plugin_outputs("BitmapMatteColorDiff", "CUDABitmapMatteColorDiff", {"value": 30.5 })