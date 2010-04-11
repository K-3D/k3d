#python

import k3d
import testing

document = k3d.new_document()

source1= k3d.plugin.create("PolyCube", document)
source1.width = 2.0
source1.height = 2.0
source1.depth = 2.0

source2 = k3d.plugin.create("PolyCube", document)
source2.width = 4.0
source2.height = 4.0
source2.depth = 4.0

modifier1 = k3d.plugin.create("PointsToParticles", document)
modifier1.width = 2.0

modifier2 = k3d.plugin.create("PointsToParticles", document)
modifier2.width = 3.0

k3d.property.connect(document, source1.get_property("output_mesh"), modifier1.get_property("input_mesh"))
k3d.property.connect(document, source2.get_property("output_mesh"), modifier2.get_property("input_mesh"))

merge_mesh = k3d.plugin.create("MergeMesh", document)
k3d.property.create(merge_mesh, "k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
k3d.property.create(merge_mesh, "k3d::mesh*", "input_mesh2", "Input Mesh 2", "")

k3d.property.connect(document, modifier1.get_property("output_mesh"), merge_mesh.get_property("input_mesh1"))
k3d.property.connect(document, modifier2.get_property("output_mesh"), merge_mesh.get_property("input_mesh2"))

testing.require_valid_mesh(document, merge_mesh.get_property("output_mesh"))
testing.require_similar_mesh(document, merge_mesh.get_property("output_mesh"), "mesh.modifier.MergeMesh.generic", 2)

