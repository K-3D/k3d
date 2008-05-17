#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "ScalePoints")

selection = k3d.deselect_all()
selection.points = k3d.component_select_all()

setup.modifier.mesh_selection = selection
setup.modifier.x = 2.0

testing.mesh_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.ScalePoints", 1)

