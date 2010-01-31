#python

import testing

setup = testing.setup_mesh_source_test("Cone")


testing.require_valid_mesh(setup.document, setup.source.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.source.get_property("output_mesh"), "mesh.source.Cone", 1)

