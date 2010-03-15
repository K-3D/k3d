#python

import testing

setup = testing.setup_mesh_source_test("UniformPolyhedron")
setup.source.type = "small_stellated_dodecahedron"
setup.source.size = 5

testing.require_valid_mesh(setup.document, setup.source.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.source.get_property("output_mesh"), "source.UniformPolyhedron", 1)

