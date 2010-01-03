#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "EulerKillEdgeMakeLoop")

setup.source.rows = 3
setup.source.columns = 3

selection = k3d.geometry.selection.create(0)
edge_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.EDGE)
k3d.geometry.primitive_selection.append(edge_selection, 0, 4294967295, 0)
k3d.geometry.primitive_selection.append(edge_selection, 7, 8, 1)
k3d.geometry.primitive_selection.append(edge_selection, 1, 2, 1)
k3d.geometry.primitive_selection.append(edge_selection, 2, 3, 1)
k3d.geometry.primitive_selection.append(edge_selection, 12, 13, 1)
k3d.geometry.primitive_selection.append(edge_selection, 14, 15, 1)
k3d.geometry.primitive_selection.append(edge_selection, 24, 25, 1)
k3d.geometry.primitive_selection.append(edge_selection, 31, 32, 1)
k3d.geometry.primitive_selection.append(edge_selection, 25, 26, 1)
k3d.geometry.primitive_selection.append(edge_selection, 35, 36, 1)
k3d.geometry.primitive_selection.append(edge_selection, 29, 30, 1)
k3d.geometry.primitive_selection.append(edge_selection, 22, 23, 1)
k3d.geometry.primitive_selection.append(edge_selection, 32, 33, 1)
k3d.geometry.primitive_selection.append(edge_selection, 10, 11, 1)
k3d.geometry.primitive_selection.append(edge_selection, 20, 21, 1)
k3d.geometry.primitive_selection.append(edge_selection, 5, 6, 1)
k3d.geometry.primitive_selection.append(edge_selection, 11, 12, 1)
setup.modifier.mesh_selection = selection


testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.EulerKillEdgeMakeLoop", 1)

