#python

import testing

setup = testing.setup_mesh_source_image_test("Polyhedron")
setup.source.number = 12
setup.source.size = 10
testing.image_comparison(setup.document, setup.camera_to_bitmap.get_property("output_bitmap"), "mesh.source.polyhedron", 0.008)

