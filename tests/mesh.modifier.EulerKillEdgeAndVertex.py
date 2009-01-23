#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "EulerKillEdgeAndVertex")

selection = k3d.mesh_selection.deselect_all()
selection.edges = [(0, 4294967295, 0), (49, 50, 1), (55, 56, 1)]

setup.modifier.mesh_selection = selection

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.EulerKillEdgeAndVertex", 1)

