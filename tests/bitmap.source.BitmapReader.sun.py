#python

import testing
setup = testing.setup_bitmap_reader_test("BitmapReader", "test_rgb_8.sun")
testing.image_comparison(setup.document, setup.reader.get_property("output_bitmap"), "BitmapReader.sun", 0)

