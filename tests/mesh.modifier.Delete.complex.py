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
selection = k3d.mesh_selection.deselect_all()
face_selection = k3d.mesh_selection.component(0, 1000, k3d.selection.type.UNIFORM)
face_selection.add_range(0, 6, 0)
face_selection.add_range(6, 7, 1)
face_selection.add_range(7, 8, 0)
face_selection.add_range(8, 9, 1)
face_selection.add_range(9, 13, 0)
face_selection.add_range(13, 15, 1)
face_selection.add_range(15, 16, 0)
face_selection.add_range(16, 17, 1)
face_selection.add_range(17, 19, 0)
face_selection.add_range(19, 20, 1)
face_selection.add_range(20, 23, 0)
point_selection = k3d.mesh_selection.component(0, 1000, k3d.selection.type.POINT)
point_selection.add_range(0, 7, 0)
point_selection.add_range(7, 8, 1)
point_selection.add_range(8, 10000, 0)
# TODO: remove this when generic selections are complete
selection.points = [(0, 7, 0), (7, 8, 1), (8, 10000, 0)]
edge_selection = k3d.mesh_selection.component(0, 1000, k3d.selection.type.SPLIT_EDGE)
edge_selection.add_range(0, 25, 0)
edge_selection.add_range(25, 26, 1)
edge_selection.add_range(26, 10000, 0)
selection.add_component(face_selection)
selection.add_component(point_selection)
selection.add_component(edge_selection)
modifier.mesh_selection = selection

document.set_dependency(modifier.get_property("input_mesh"), varying_colors.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.Delete.complex", 1)
