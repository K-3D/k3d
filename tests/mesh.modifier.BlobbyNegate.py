#python

import k3d
import testing

document = k3d.new_document()

source1 = document.new_node("BlobbyEllipsoid")
source1.color = k3d.color(1, 0, 0)

modifier = document.new_node("BlobbyNegate")

document.set_dependency(modifier.get_property("input_mesh"), source1.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.BlobbyNegate", 5)

