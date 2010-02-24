#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "CollapseFaces")

selection = k3d.selection.set()
primitive_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.FACE)
k3d.geometry.primitive_selection.append(primitive_selection, 0, 1, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 13, 14, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 17, 18, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 19, 20, 1)
k3d.geometry.primitive_selection.append(primitive_selection, 23, 24, 1)

setup.modifier.mesh_selection = selection

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CollapseFaces", 1)

