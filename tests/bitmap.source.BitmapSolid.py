#python

import k3d
import testing

doc = k3d.new_document()
source = doc.new_node("BitmapSolid")
source.color = k3d.color(1, 1, 0)

testing.image_comparison(doc, source.get_property("output_bitmap"), "BitmapSolid", 0)

