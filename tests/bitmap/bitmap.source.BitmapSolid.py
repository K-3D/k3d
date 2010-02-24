#python

import k3d
import testing

setup = testing.setup_bitmap_source_test("BitmapSolid")
setup.source.color = k3d.color(1, 1, 0)

testing.require_similar_bitmap(setup.document, setup.source.get_property("output_bitmap"), "BitmapSolid", 0)

