#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCylinder", "BulgePoints")

setup.source.radius = 1
setup.modifier.bulge_factor = 5
setup.modifier.displace_y = False
setup.modifier.displace_z = False

testing.mesh_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.BulgePoints", 0)

