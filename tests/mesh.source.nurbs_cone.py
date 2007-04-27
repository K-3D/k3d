#python

import testing

setup = testing.setup_mesh_source_test("NurbsCone")
testing.image_comparison(setup.document, setup.camera_to_bitmap.get_property("output_bitmap"), "mesh.source.nurbs_cone", 0.006)

