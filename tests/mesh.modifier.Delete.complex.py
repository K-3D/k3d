#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("K3DMeshReader", "Delete")
setup.source.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/polyhedron.hole.k3d")
setup.source.center = False
setup.source.scale_to_size = False

selection = k3d.geometry.selection.create(0)

point_selection = k3d.geometry.point_selection.create(selection)
k3d.geometry.point_selection.append(point_selection, 0, 10000, 0)
k3d.geometry.point_selection.append(point_selection, 7, 8, 1)

face_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.FACE)
k3d.geometry.primitive_selection.append(face_selection, 0, 10000, 0)
k3d.geometry.primitive_selection.append(face_selection, 6, 7, 1)
k3d.geometry.primitive_selection.append(face_selection, 8, 9, 1)
k3d.geometry.primitive_selection.append(face_selection, 13, 15, 1)
k3d.geometry.primitive_selection.append(face_selection, 16, 17, 1)
k3d.geometry.primitive_selection.append(face_selection, 19, 20, 1)

edge_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.EDGE)
k3d.geometry.primitive_selection.append(edge_selection, 0, 10000, 0)
k3d.geometry.primitive_selection.append(edge_selection, 25, 26, 1)

setup.modifier.mesh_selection = selection

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.Delete.complex", 1)

