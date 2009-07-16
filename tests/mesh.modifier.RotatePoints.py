#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "RotatePoints")

selection = k3d.geometry.uniform_selection(0)
selection.points = k3d.geometry.point_selection.uniform(selection, 1)

setup.modifier.mesh_selection = selection
setup.modifier.z = 1.0

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.RotatePoints", 2)

