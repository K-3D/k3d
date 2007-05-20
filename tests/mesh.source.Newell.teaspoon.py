#python

import testing

setup = testing.setup_mesh_source_test("Newell")
setup.source.type = "teaspoon"
testing.mesh_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.Newell.teaspoon", 1)

