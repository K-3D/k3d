#python

import k3d
import testing

setup = testing.setup_bitmap_source_test("BitmapChecker")
setup.source.color1 = k3d.color(1, 1, 0)
setup.source.color2 = k3d.color(0, 0, 1)

testing.require_similar_bitmap(setup.document, setup.source.get_property("output_bitmap"), "BitmapChecker", 0)

