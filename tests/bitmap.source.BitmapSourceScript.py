#python

import k3d
import testing

doc = k3d.new_document()
source = doc.new_node("BitmapSourceScript")
testing.image_comparison(doc, source.get_property("output_bitmap"), "BitmapSourceScript", 0)

