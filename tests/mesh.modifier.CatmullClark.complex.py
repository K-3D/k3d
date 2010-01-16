#python

import k3d
import testing

document = k3d.new_document()

reader = document.new_node("K3DMeshReader")
# load a mesh that has multiple polyhedra, triangles, quads, n-sided polygons and holes. (i.e. a mesh from hell)
reader.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/polyhedron.hole.k3d")
reader.center = False
reader.scale_to_size = False

modifier = document.new_node("CatmullClark")

modifier.level = 2

document.set_dependency(modifier.get_property("input_mesh"), reader.get_property("output_mesh"))

testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(document, modifier.get_property("output_mesh"), "mesh.modifier.CatmullClark.complex", 2)
