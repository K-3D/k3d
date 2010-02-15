#python

import k3d
import testing

setup = testing.setup_bitmap_modifier_test("BitmapReader", "BitmapMatteInvert")
setup.source.file = k3d.filesystem.generic_path(testing.source_path() + "/bitmaps/" + "test_rgb_8_alpha.png")

testing.require_similar_bitmap(setup.document, setup.modifier.get_property("output_bitmap"), "BitmapMatteInvert", 0)
