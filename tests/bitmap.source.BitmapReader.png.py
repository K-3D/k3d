#python

import testing
setup = testing.setup_bitmap_reader_test("BitmapReader", "test_rgb_8.png")
testing.image_comparison(setup.document, setup.reader.get_property("output_bitmap"), "BitmapReader.png", 0)

