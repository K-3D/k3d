#python

import testing
setup = testing.setup_bitmap_reader_test("JPEGBitmapReader", "test_rgb_8.jpg")
testing.image_comparison(setup.document, setup.reader.get_property("output_bitmap"), "JPEGBitmapReader", 0.09)

