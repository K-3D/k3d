#python

import testing

setup = testing.setup_mesh_source_test("PolyCube")
testing.mesh_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.poly_cube", 1)

