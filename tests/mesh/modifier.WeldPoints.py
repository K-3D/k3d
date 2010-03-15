#python

import k3d
import testing

document = k3d.new_document()

source1 = k3d.plugin.create("PolyCube", document)
source2 = k3d.plugin.create("PolyCube", document)

merge = k3d.plugin.create("MergeMesh", document)
k3d.property.create(merge, "k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
k3d.property.create(merge, "k3d::mesh*", "input_mesh2", "Input Mesh 2", "")
k3d.property.connect(document, source1.get_property("output_mesh"), merge.get_property("input_mesh1"))
k3d.property.connect(document, source1.get_property("output_mesh"), merge.get_property("input_mesh2"))

weld = k3d.plugin.create("WeldPoints", document)
weld.distance = 0.5
k3d.property.connect(document, merge.get_property("output_mesh"), weld.get_property("input_mesh"))

testing.require_valid_mesh(document, weld.get_property("output_mesh"))
testing.require_similar_mesh(document, weld.get_property("output_mesh"), "mesh.modifier.WeldPoints", 1)

