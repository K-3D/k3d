#python
import testing

testing.bitmap_compare_plugin_outputs("BitmapThreshold", "CUDABitmapThreshold", {"red_threshold": 0.0, "green_threshold": 0.0, "blue_threshold": 0.0, "alpha_threshold": 0.0 })
testing.bitmap_compare_plugin_outputs("BitmapThreshold", "CUDABitmapThreshold", {"red_threshold": 0.5, "green_threshold": 0.5, "blue_threshold": 0.5, "alpha_threshold": 2.0 })