#python
import testing

testing.bitmap_compare_plugin_outputs("BitmapGamma", "CUDABitmapGamma", {"value": 0.0 })
testing.bitmap_compare_plugin_outputs("BitmapGamma", "CUDABitmapGamma", {"value": 30.5 })