#python

import k3d
import testing

setup = testing.setup_bitmap_modifier_test("BitmapReader", "BitmapGamma")
setup.source.file = k3d.filesystem.generic_path(testing.source_path() + "/bitmaps/" + "test_rgb_8.png")
setup.modifier.gamma = 2.2

testing.require_similar_bitmap(setup.document, setup.modifier.get_property("output_bitmap"), "BitmapGamma", 0)
