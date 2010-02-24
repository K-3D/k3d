#python

import testing
setup = testing.setup_bitmap_reader_test("BitmapReader", "test_rgb_8.png")
testing.require_similar_bitmap(setup.document, setup.source.get_property("output_bitmap"), "BitmapReader.png", 0)

