#python

import testing

setup = testing.setup_mesh_source_test("Sphere")

testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.Sphere", 1)

