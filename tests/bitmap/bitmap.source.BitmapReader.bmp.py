#python

import testing
setup = testing.setup_bitmap_reader_test("BitmapReader", "test_rgb_8.bmp")
testing.require_similar_bitmap(setup.document, setup.source.get_property("output_bitmap"), "BitmapReader.bmp", 0)

