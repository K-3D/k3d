#python

import k3d
import testing

setup = testing.setup_bitmap_modifier_test("BitmapReader", "BitmapSubtract")
setup.source.file = k3d.filesystem.generic_path(testing.source_path() + "/bitmaps/" + "test_rgb_8.png")
setup.modifier.value = 0.5

testing.require_similar_bitmap(setup.document, setup.modifier.get_property("output_bitmap"), "BitmapSubtract", 0.001)
