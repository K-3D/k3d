#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCylinder", "TaperPoints")

selection = k3d.geometry.selection.create(0)
selection.points = k3d.geometry.point_selection.create(selection, 1)

setup.modifier.mesh_selection = selection
setup.modifier.taper_factor = 1

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.TaperPoints", 1, True)

