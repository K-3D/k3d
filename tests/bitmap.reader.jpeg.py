#python

import testing
setup = testing.setup_bitmap_reader_test("JPEGBitmapReader", "test_rgb_8.jpg")
testing.bitmap_size_comparison(setup.reader.output_bitmap, 200, 150)

