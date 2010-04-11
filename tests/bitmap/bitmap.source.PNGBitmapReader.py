#python

import testing
setup = testing.setup_bitmap_reader_test("PNGBitmapReader", "test_rgb_8.png")
testing.require_bitmap_size(setup.source.output_bitmap, 200, 150)

