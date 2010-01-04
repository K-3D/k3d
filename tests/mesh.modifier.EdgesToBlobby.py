#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "EdgesToBlobby")


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.EdgesToBlobby", 1)

