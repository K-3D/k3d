#python
import testing

testing.bitmap_compare_plugin_outputs("BitmapAdd", "CUDABitmapAdd", {"value": 0.0 })
testing.bitmap_compare_plugin_outputs("BitmapAdd", "CUDABitmapAdd", {"value": 30.5 })