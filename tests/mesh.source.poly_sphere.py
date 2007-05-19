#python

import testing

setup = testing.setup_mesh_source_test("PolySphere")
testing.mesh_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.poly_sphere", 1)

