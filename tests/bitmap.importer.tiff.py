#python

import k3d

doc = k3d.application.new_document()
bitmap_node = doc.new_node("BitmapReader")
bitmap_node.file = "@CMAKE_CURRENT_SOURCE_DIR@/bitmaps/test_rgb_8.tif"
bitmap = bitmap_node.output_bitmap
if bitmap.width != 200 or bitmap.height != 150:
	raise "Error loading test bitmap"

