#python

import k3d
import testing

document = k3d.new_document()

reader = document.new_node("K3DMeshReader")
# load a mesh that has multiple polyhedra, triangles, quads, n-sided polygons and holes. (i.e. a mesh from hell)
reader.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/testmesh.polyhedra.hole.k3d")
reader.center = False
reader.scale_to_size = False

varying_colors = document.new_node("RandomColors")

document.set_dependency(varying_colors.get_property("input_mesh"), reader.get_property("output_mesh"))

modifier = document.new_node("Delete")

# select some faces, distributed along polyhedra
selection = k3d.geometry.selection.create(0)

point_selection = k3d.geometry.point_selection.create(selection)
k3d.geometry.point_selection.append(point_selection, 0, 7, 0)
k3d.geometry.point_selection.append(point_selection, 7, 8, 1)
k3d.geometry.point_selection.append(point_selection, 8, 10000, 0)

face_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.UNIFORM)
k3d.geometry.primitive_selection.append(face_selection, 0, 6, 0)
k3d.geometry.primitive_selection.append(face_selection, 6, 7, 1)
k3d.geometry.primitive_selection.append(face_selection, 7, 8, 0)
k3d.geometry.primitive_selection.append(face_selection, 8, 9, 1)
k3d.geometry.primitive_selection.append(face_selection, 9, 13, 0)
k3d.geometry.primitive_selection.append(face_selection, 13, 15, 1)
k3d.geometry.primitive_selection.append(face_selection, 15, 16, 0)
k3d.geometry.primitive_selection.append(face_selection, 16, 17, 1)
k3d.geometry.primitive_selection.append(face_selection, 17, 19, 0)
k3d.geometry.primitive_selection.append(face_selection, 19, 20, 1)
k3d.geometry.primitive_selection.append(face_selection, 20, 23, 0)

edge_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.SPLIT_EDGE)
k3d.geometry.primitive_selection.append(edge_selection, 0, 25, 0)
k3d.geometry.primitive_selection.append(edge_selection, 25, 26, 1)
k3d.geometry.primitive_selection.append(edge_selection, 26, 10000, 0)

modifier.mesh_selection = selection

document.set_dependency(modifier.get_property("input_mesh"), varying_colors.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.Delete.complex", 1)
