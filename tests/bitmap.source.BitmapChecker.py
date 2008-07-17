#python

import k3d
import testing

doc = k3d.new_document()
source = doc.new_node("BitmapChecker")
source.color1 = k3d.color(1, 1, 0)
source.color2 = k3d.color(0, 0, 1)

testing.image_comparison(doc, source.get_property("output_bitmap"), "BitmapChecker", 0)

