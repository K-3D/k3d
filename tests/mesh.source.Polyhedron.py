#python

import testing

setup = testing.setup_mesh_source_test("Polyhedron")
setup.source.number = 12
setup.source.size = 10

testing.require_valid_mesh(setup.document, setup.source.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.Polyhedron", 1, testing.platform_specific)

