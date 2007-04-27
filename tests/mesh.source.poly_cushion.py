#python

import testing

setup = testing.setup_mesh_source_test("PolyCushion")
testing.image_comparison(setup.document, setup.camera_to_bitmap.get_property("output_bitmap"), "mesh.source.poly_cushion", 0.016)

