#python

import k3d
import testing

setup = testing.setup_bitmap_modifier_test("BitmapReader", "BitmapThreshold")
setup.source.file = k3d.filesystem.generic_path(testing.source_path() + "/bitmaps/" + "test_rgb_8_alpha.png")
setup.modifier.red_threshold = 0.5 
setup.modifier.blue_threshold = 0.5 
setup.modifier.alpha_threshold = 0.5 

testing.require_similar_bitmap(setup.document, setup.modifier.get_property("output_bitmap"), "BitmapThreshold", 0)
