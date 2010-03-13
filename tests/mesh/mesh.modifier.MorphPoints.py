#python

import k3d
import testing

document = k3d.new_document()

source1 = k3d.plugin.create("PolyCone", document)
source2 = k3d.plugin.create("PolySphere", document)
source3 = k3d.plugin.create("PolyCube", document)
modifier = k3d.plugin.create("MorphPoints", document)

modifier.create_property("k3d::mesh*", "input1", "input1", "input1")
modifier.create_property("k3d::mesh*", "input2", "input2", "input2")
modifier.create_property("k3d::double_t", "amount1", "amount1", "amount1")
modifier.create_property("k3d::double_t", "amount2", "amount2", "amount2")

k3d.property.connect(document, source1.get_property("output_mesh"), modifier.get_property("input_mesh"))
k3d.property.connect(document, source2.get_property("output_mesh"), modifier.get_property("input1"))
k3d.property.connect(document, source3.get_property("output_mesh"), modifier.get_property("input2"))

selection = k3d.geometry.selection.create(0)
point_selection = k3d.geometry.point_selection.create(selection, 1)

modifier.mesh_selection = selection

modifier.amount1 = 0.1
modifier.amount2 = 0.1 

testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.MorphPoints", 4)

