#python

import testing

setup = testing.setup_mesh_source_test("Polyhedron")
setup.source.number = 12
setup.source.size = 10
testing.mesh_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.polyhedron", 1)

