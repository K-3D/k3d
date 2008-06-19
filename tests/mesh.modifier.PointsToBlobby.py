#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "PointsToBlobby")

testing.mesh_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.PointsToBlobby", 1)

