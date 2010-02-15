#python

import k3d
import testing

setup = testing.setup_bitmap_modifier_test("BitmapReader", "BitmapColorMonochrome")
setup.source.file = k3d.filesystem.generic_path(testing.source_path() + "/bitmaps/" + "test_rgb_8.png")

testing.require_similar_bitmap(setup.document, setup.modifier.get_property("output_bitmap"), "BitmapColorMonochrome", 0)
