#python

import testing

setup = testing.setup_mesh_source_test("GTSPolySphere")
testing.mesh_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.gts_poly_sphere", 1)

