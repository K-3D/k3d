#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test2("PolyCube", "TranslatePoints", "CenterPoints")

selection = k3d.geometry.selection.create(0)
selection.points = k3d.geometry.point_selection.create(selection, 1)

setup.modifier1.mesh_selection = selection
setup.modifier1.x = 1.0
setup.modifier1.y = 2.0
setup.modifier1.z = 3.0

setup.modifier2.mesh_selection = selection
setup.modifier2.center_x = True
setup.modifier2.center_y = True
setup.modifier2.center_z = True


testing.require_valid_mesh(setup.document, setup.modifier2.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier2.get_property("output_mesh"), "mesh.modifier.CenterPoints", 1)

