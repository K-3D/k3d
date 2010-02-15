#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "ExtrudeFaces")

setup.source.rows = 3
setup.source.columns = 3

selection = k3d.geometry.selection.create(0)
face_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.FACE)
k3d.geometry.primitive_selection.append(face_selection, 4, 5, 1)
setup.modifier.mesh_selection = selection
setup.modifier.distance = 5


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.ExtrudeFaces", 2)

