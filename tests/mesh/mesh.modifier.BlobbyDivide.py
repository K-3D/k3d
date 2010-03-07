#python

import k3d
import testing

document = k3d.new_document()

source1 = k3d.plugin.create("BlobbyEllipsoid", document)
source1.color = k3d.color(1, 0, 0)

source2 = k3d.plugin.create("BlobbySegment", document)
source2.color = k3d.color(1, 1, 0)

modifier = k3d.plugin.create("BlobbyDivide", document)

document.set_dependency(modifier.get_property("input_a"), source1.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_b"), source2.get_property("output_mesh"))


testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.BlobbyDivide", 5)

