#python

import testing

setup = testing.setup_mesh_source_image_test("PolySphere")
testing.image_comparison(setup.document, setup.camera_to_bitmap.get_property("output_bitmap"), "mesh.source.poly_sphere", 0.017)

