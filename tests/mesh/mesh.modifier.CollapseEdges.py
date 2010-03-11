#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "CollapseEdges")

selection = k3d.selection.set()
primitive_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.EDGE)
k3d.geometry.primitive_selection.append(primitive_selection, 10, 11, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 28, 29, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 30, 31, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 48, 49, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 48, 49, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 50, 51, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 68, 69, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 70, 71, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 88, 89, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 90, 91, 1)

k3d.geometry.primitive_selection.append(primitive_selection, 43, 44, 1)

k3d.geometry.primitive_selection.append(primitive_selection, 18, 19, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 36, 37, 1)

k3d.geometry.primitive_selection.append(primitive_selection, 82, 83, 1)

setup.modifier.mesh_selection = selection

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CollapseEdges", 2)

