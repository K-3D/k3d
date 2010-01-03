#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "SelectCube")

setup.modifier.x1 = 0.0

testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.SelectCube", 1)

