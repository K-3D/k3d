#python

import testing
setup = testing.setup_bitmap_reader_test("BitmapReader", "test_rgb_8.jpg")
testing.require_similar_bitmap(setup.document, setup.source.get_property("output_bitmap"), "BitmapReader.jpeg", 0.09)

