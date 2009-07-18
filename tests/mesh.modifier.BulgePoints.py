#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCylinder", "BulgePoints")

setup.source.radius = 1

selection = k3d.geometry.selection.create(0)
selection.points = k3d.geometry.point_selection.create(selection, 1)

setup.modifier.mesh_selection = selection
setup.modifier.bulge_factor = 5
setup.modifier.displace_y = False
setup.modifier.displace_z = False

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.BulgePoints", 2)

