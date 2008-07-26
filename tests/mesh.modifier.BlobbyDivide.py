#python

import k3d
import testing

document = k3d.new_document()

source1 = document.new_node("BlobbyEllipsoid")
source1.color = k3d.color(1, 0, 0)

source2 = document.new_node("BlobbySegment")
source2.color = k3d.color(1, 1, 0)

modifier = document.new_node("BlobbyDivide")

document.set_dependency(modifier.get_property("input_a"), source1.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_b"), source2.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.BlobbyDivide", 5)

