#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "BevelPoints")

selection = k3d.geometry.selection.create(0)
selection.points = k3d.geometry.point_selection.uniform(selection, 1)
setup.modifier.mesh_selection = selection

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.BevelPoints", 1)

