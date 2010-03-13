#python

import k3d
import testing

document = k3d.new_document()

source1 = k3d.plugin.create("BlobbyEllipsoid", document)
source1.color = k3d.color(1, 0, 0)

source2 = k3d.plugin.create("BlobbySegment", document)
source2.color = k3d.color(1, 1, 0)

modifier = k3d.plugin.create("BlobbyMinimum", document)
modifier.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
modifier.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")

k3d.property.connect(document, source1.get_property("output_mesh"), modifier.get_property("input_mesh1"))
k3d.property.connect(document, source2.get_property("output_mesh"), modifier.get_property("input_mesh2"))


testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.BlobbyMinimum", 5)

