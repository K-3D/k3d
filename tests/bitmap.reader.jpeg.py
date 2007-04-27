#python

import k3d

doc = k3d.application.new_document()
bitmap_object = doc.new_node("JPEGBitmapReader")
bitmap_object.file = "@CMAKE_CURRENT_SOURCE_DIR@/bitmaps/test_rgb_8.jpg"
bitmap = bitmap_object.output_bitmap
if bitmap.width != 200 or bitmap.height != 150:
	raise "Error loading test bitmap"

