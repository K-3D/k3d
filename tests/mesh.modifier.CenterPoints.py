#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test2("PolyCube", "TranslatePoints", "CenterPoints")

selection = k3d.geometry.uniform_selection(0)
selection.points = k3d.geometry.point_selection.uniform(selection, 1)

setup.modifier1.mesh_selection = selection
setup.modifier1.x = 1.0
setup.modifier1.y = 2.0
setup.modifier1.z = 3.0

setup.modifier2.mesh_selection = selection
setup.modifier2.center_x = True
setup.modifier2.center_y = True
setup.modifier2.center_z = True

testing.mesh_comparison_to_reference(setup.document, setup.modifier2.get_property("output_mesh"), "mesh.modifier.CenterPoints", 1)

