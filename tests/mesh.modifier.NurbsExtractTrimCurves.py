#python

import testing
import k3d

document = k3d.new_document()

patch = document.new_node("NurbsGrid")
curve = document.new_node("NurbsCircle")
merge_mesh = document.new_node("MergeMesh")
modifier = document.new_node("NurbsAddTrimCurve")
extract_trim = document.new_node("NurbsExtractTrimCurves")

merge_mesh.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
merge_mesh.create_property("k3d::mesh*", "input_mesh2", "Input Mesh 2", "")

modifier.mesh_selection = k3d.geometry.selection.create(1)
modifier.offset_u = 0.5
modifier.offset_v = 0.5
modifier.delete_curve = True

extract_trim.mesh_selection = k3d.geometry.selection.create(1)

document.set_dependency(merge_mesh.get_property("input_mesh1"), patch.get_property("output_mesh"))
document.set_dependency(merge_mesh.get_property("input_mesh2"), curve.get_property("output_mesh"))
document.set_dependency(modifier.get_property("input_mesh"), merge_mesh.get_property("output_mesh"))
document.set_dependency(extract_trim.get_property("input_mesh"), modifier.get_property("output_mesh"))


testing.require_valid_primitives(document, extract_trim.get_property("output_mesh"))
testing.mesh_reference_comparison(document, extract_trim.get_property("output_mesh"), "mesh.modifier.NurbsExtractTrimCurves", 1)
