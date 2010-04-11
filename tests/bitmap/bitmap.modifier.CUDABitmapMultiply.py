#python
import testing

testing.bitmap_compare_plugin_outputs("BitmapMultiply", "CUDABitmapMultiply", {"value": 1.0 })
testing.bitmap_compare_plugin_outputs("BitmapMultiply", "CUDABitmapMultiply", {"value": 0.25 })

