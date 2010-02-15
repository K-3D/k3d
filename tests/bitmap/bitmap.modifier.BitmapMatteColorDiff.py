#python

import k3d
import testing

setup = testing.setup_bitmap_modifier_test("BitmapReader", "BitmapMatteColorDiff")
setup.source.file = k3d.filesystem.generic_path(testing.source_path() + "/bitmaps/" + "test_rgb_8_alpha.png")
setup.modifier.threshold = 0.95

testing.require_similar_bitmap(setup.document, setup.modifier.get_property("output_bitmap"), "BitmapMatteColorDiff", 0)
