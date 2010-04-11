#python

import testing
setup = testing.setup_bitmap_reader_test("TIFFBitmapReader", "test_rgb_8.tif")
testing.require_similar_bitmap(setup.document, setup.source.get_property("output_bitmap"), "TIFFBitmapReader", 0)

