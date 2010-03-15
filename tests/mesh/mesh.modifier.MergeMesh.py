#python

import k3d
import testing

document = k3d.new_document()

source1 = k3d.plugin.create("BlobbyEllipsoid", document)
source2 = k3d.plugin.create("LinearLissajousCurve", document)
source3 = k3d.plugin.create("Newell", document)
source4 = k3d.plugin.create("NurbsCurve", document)
source5 = k3d.plugin.create("NurbsGrid", document)
source6 = k3d.plugin.create("PolyCube", document)
source7 = k3d.plugin.create("QuadricCone", document)
source8 = k3d.plugin.create("PolyCylinder", document)
source9 = k3d.plugin.create("NurbsGrid", document)
modifier = k3d.plugin.create("MergeMesh", document)

k3d.property.create(modifier, "k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
k3d.property.create(modifier, "k3d::mesh*", "input_mesh2", "Input Mesh 2", "")
k3d.property.create(modifier, "k3d::mesh*", "input_mesh3", "Input Mesh 3", "")
k3d.property.create(modifier, "k3d::mesh*", "input_mesh4", "Input Mesh 4", "")
k3d.property.create(modifier, "k3d::mesh*", "input_mesh5", "Input Mesh 5", "")
k3d.property.create(modifier, "k3d::mesh*", "input_mesh6", "Input Mesh 6", "")
k3d.property.create(modifier, "k3d::mesh*", "input_mesh7", "Input Mesh 7", "")
k3d.property.create(modifier, "k3d::mesh*", "input_mesh8", "Input Mesh 8", "")
k3d.property.create(modifier, "k3d::mesh*", "input_mesh9", "Input Mesh 9", "")

k3d.property.connect(document, source1.get_property("output_mesh"), modifier.get_property("input_mesh1"))
k3d.property.connect(document, source2.get_property("output_mesh"), modifier.get_property("input_mesh2"))
k3d.property.connect(document, source3.get_property("output_mesh"), modifier.get_property("input_mesh3"))
k3d.property.connect(document, source4.get_property("output_mesh"), modifier.get_property("input_mesh4"))
k3d.property.connect(document, source5.get_property("output_mesh"), modifier.get_property("input_mesh5"))
k3d.property.connect(document, source6.get_property("output_mesh"), modifier.get_property("input_mesh6"))
k3d.property.connect(document, source7.get_property("output_mesh"), modifier.get_property("input_mesh7"))
k3d.property.connect(document, source8.get_property("output_mesh"), modifier.get_property("input_mesh8"))
k3d.property.connect(document, source9.get_property("output_mesh"), modifier.get_property("input_mesh9"))


testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.MergeMesh", 32)

