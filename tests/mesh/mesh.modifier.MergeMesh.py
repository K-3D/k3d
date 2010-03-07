#python

import k3d
import testing

document = k3d.new_document()

source1 = k3d.plugin.create("PolyCube", document)
source2 = k3d.plugin.create("PolyCube", document)
source3 = k3d.plugin.create("NurbsSphere", document)
source4 = k3d.plugin.create("NurbsSphere", document)
source5 = k3d.plugin.create("NurbsCurve", document)
source6 = k3d.plugin.create("NurbsCurve", document)
source7 = k3d.plugin.create("LinearLissajousCurve", document)
source8 = k3d.plugin.create("Newell", document)
source9 = k3d.plugin.create("Newell", document)
modifier = k3d.plugin.create("MergeMesh", document)

modifier.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
modifier.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")
modifier.create_property("k3d::mesh*", "input_mesh3", "Input Mesh 3", "")
modifier.create_property("k3d::mesh*", "input_mesh4", "Input Mesh 4", "")
modifier.create_property("k3d::mesh*", "input_mesh5", "Input Mesh 5", "")
modifier.create_property("k3d::mesh*", "input_mesh6", "Input Mesh 6", "")
modifier.create_property("k3d::mesh*", "input_mesh7", "Input Mesh 7", "")
modifier.create_property("k3d::mesh*", "input_mesh8", "Input Mesh 8", "")
modifier.create_property("k3d::mesh*", "input_mesh9", "Input Mesh 9", "")

document.set_dependency(modifier.get_property("input_mesh1"), source1.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh2"), source2.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh3"), source3.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh4"), source4.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh5"), source5.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh6"), source6.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh7"), source7.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh8"), source8.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh9"), source9.get_property("output_mesh"))


testing.require_valid_mesh(document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(document, modifier.get_property("output_mesh"), "mesh.modifier.MergeMesh", 1, testing.platform_specific)

