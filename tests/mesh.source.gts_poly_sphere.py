#python

import testing

setup = testing.setup_mesh_source_test("GTSPolySphere")
testing.image_comparison(setup.document, setup.camera_to_bitmap.get_property("output_bitmap"), "mesh.source.gts_poly_sphere", 0.015)

