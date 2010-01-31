#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("K3DMeshReader", "SubdivideFaces")

# load a mesh that has multiple polyhedra, triangles, quads, n-sided polygons and holes. (i.e. a mesh from hell)
setup.source.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/polyhedron.hole.k3d")
setup.source.center = False
setup.source.scale_to_size = False

# select some faces, distributed along polyhedra
selection = k3d.geometry.selection.create(0)
face_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.FACE)
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

setup.modifier.mesh_selection = selection
setup.modifier.subdivision_type = "center"

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.SubdivideFaces.Center", 1)
