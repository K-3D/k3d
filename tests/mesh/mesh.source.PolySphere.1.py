#python

import testing

setup = testing.setup_mesh_source_test("PolySphere")

setup.source.type = "sphere"


testing.require_valid_mesh(setup.document, setup.source.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.source.get_property("output_mesh"), "mesh.source.PolySphere.1", 2)

