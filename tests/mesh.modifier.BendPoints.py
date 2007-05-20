#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCylinder", "BendPoints")

setup.source.radius = 1
setup.modifier.angle = 90
setup.modifier.around = "y"

testing.mesh_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.BendPoints", 0)

