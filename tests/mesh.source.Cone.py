#python

import testing

setup = testing.setup_mesh_source_test("Cone")

testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.Cone", 1)

