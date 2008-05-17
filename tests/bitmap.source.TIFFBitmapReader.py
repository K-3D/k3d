#python

import testing
setup = testing.setup_bitmap_reader_test("TIFFBitmapReader", "test_rgb_8.tif")
testing.image_comparison(setup.document, setup.reader.get_property("output_bitmap"), "TIFFBitmapReader", 0)

