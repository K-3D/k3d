#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCylinder", "BendPoints")

setup.source.radius = 1

selection = k3d.geometry.uniform_selection(0)
point_selection = k3d.geometry.point_selection.uniform(selection, 1)

setup.modifier.mesh_selection = selection
setup.modifier.angle = 90
setup.modifier.around = "y"

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.BendPoints", 32)

