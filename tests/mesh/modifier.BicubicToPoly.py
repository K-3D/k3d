#python

import testing

setup = testing.setup_mesh_modifier_test("Newell", "BicubicToPoly")

setup.source.type = "teapot"
setup.modifier.subdivisions = 3


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.BicubicToPoly", 2)

