#python
import testing

testing.bitmap_compare_plugin_outputs("BitmapSubtract", "CUDABitmapSubtract", {"value": 0.0 })
testing.bitmap_compare_plugin_outputs("BitmapSubtract", "CUDABitmapSubtract", {"value": 30.5 })
