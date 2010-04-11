#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "EulerKillEdgeAndVertex")

selection = k3d.geometry.selection.create(1)
edge_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.EDGE)
k3d.geometry.primitive_selection.append(edge_selection, 0, 4294967295, 0)
k3d.geometry.primitive_selection.append(edge_selection, 49, 50, 1)
k3d.geometry.primitive_selection.append(edge_selection, 55, 56, 1)
setup.modifier.mesh_selection = selection


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.EulerKillEdgeAndVertex", 1)

