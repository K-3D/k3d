#python
import testing

testing.bitmap_compare_plugin_outputs("BitmapColorMonochrome", "CUDABitmapColorMonochrome", {"red_weight": 1.0, "green_weight":1.0, "blue_weight":1.0 })
testing.bitmap_compare_plugin_outputs("BitmapColorMonochrome", "CUDABitmapColorMonochrome", {"red_weight": 0.0, "green_weight":1.0, "blue_weight":0.5 })