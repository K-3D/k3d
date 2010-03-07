#python

import k3d
import testing

document = k3d.new_document()

source1 = k3d.plugin.create("BlobbyEllipsoid", document)
source1.color = k3d.color(1, 0, 0)

modifier = k3d.plugin.create("BlobbyNegate", document)

document.set_dependency(modifier.get_property("input_mesh"), source1.get_property("output_mesh"))


testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.BlobbyNegate", 5)

