#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "Delete")

selection = k3d.geometry.selection.create(0)
edge_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.EDGE)
k3d.geometry.primitive_selection.append(edge_selection, 0, 2, 1)
k3d.geometry.primitive_selection.append(edge_selection, 32, 33, 1)
k3d.geometry.primitive_selection.append(edge_selection, 48, 52, 1)
setup.modifier.mesh_selection = selection


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.Delete.edge", 1)

