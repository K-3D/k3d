#python

import testing

setup = testing.setup_mesh_source_test("PolyCube")
testing.image_comparison(setup.document, setup.camera_to_bitmap.get_property("output_bitmap"), "mesh.source.poly_cube", 0.009)

