#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "Delete")

selection = k3d.geometry.selection.create(0)
face_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.FACE)
k3d.geometry.primitive_selection.append(face_selection, 0, 2, 1)
k3d.geometry.primitive_selection.append(face_selection, 12, 13, 1)
setup.modifier.mesh_selection = selection


testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.Delete.face", 1)

