#python

import testing
import k3d

document = k3d.new_document()
setup = testing.setup_mesh_reader_test("K3DMeshReader","mesh.modifier.NurbsCloseCurveReference.k3d")

selection = k3d.geometry.selection.create(0)
nurbs_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.UNIFORM)
k3d.geometry.primitive_selection.append(nurbs_selection, 1)

modifier = setup.document.new_node("NurbsCloseCurve")
modifier.mesh_selection = selection
modifier.keep_ends = False

document.set_dependency(modifier.get_property("input_mesh"), setup.reader.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsCloseCurve1", 1)

