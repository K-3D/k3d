#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "EulerKillEdgeMakeLoop")

setup.source.rows = 3
setup.source.columns = 3

selection = k3d.mesh_selection.deselect_all()
selection.edges = [(0, 4294967295, 0), (7, 8, 1), (1, 2, 1), (2, 3, 1), (12, 13, 1), (14, 15, 1), (24, 25, 1), (31, 32, 1), (25, 26, 1), (35, 36, 1), (29, 30, 1), (22, 23, 1), (32, 33, 1), (10, 11, 1), (20, 21, 1), (5, 6, 1), (11, 12, 1)]

setup.modifier.mesh_selection = selection

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.EulerKillEdgeMakeLoop", 1)

