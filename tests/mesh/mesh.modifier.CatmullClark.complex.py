#python

import k3d
import testing

document = k3d.new_document()

reader = k3d.plugin.create("K3DMeshReader", document)
# load a mesh that has multiple polyhedra, triangles, quads, n-sided polygons and holes. (i.e. a mesh from hell)
reader.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/polyhedron.hole.k3d")
reader.center = False
reader.scale_to_size = False

modifier = k3d.plugin.create("CatmullClark", document)

modifier.level = 2

k3d.property.connect(document, reader.get_property("output_mesh"), modifier.get_property("input_mesh"))

testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.CatmullClark.complex", 2)
