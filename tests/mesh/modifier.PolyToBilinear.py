#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "PolyToBilinear")


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.PolyToBilinear", 1)

