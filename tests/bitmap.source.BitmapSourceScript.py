#python

import k3d
import testing

setup = testing.setup_bitmap_source_test("BitmapSourceScript")
testing.require_similar_bitmap(setup.document, setup.source.get_property("output_bitmap"), "BitmapSourceScript", 0)

