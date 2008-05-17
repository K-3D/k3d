#python

import testing

setup = testing.setup_mesh_source_test("Newell")
setup.source.type = "teacup"
testing.mesh_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.Newell.teacup", 1)

