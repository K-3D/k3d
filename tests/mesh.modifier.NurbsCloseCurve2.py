#python

import testing
import k3d

document = k3d.new_document()
setup = testing.setup_mesh_reader_test("K3DMeshReader","mesh.modifier.NurbsCloseCurveReference.k3d")

selection = k3d.deselect_all()
selection.nurbs_curves = k3d.component_select_all()

modifier = setup.document.new_node("NurbsCloseCurve")
modifier.mesh_selection = selection
modifier.keep_ends = True

document.set_dependency(modifier.get_property("input_mesh"), setup.reader.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsCloseCurve2", 1)

