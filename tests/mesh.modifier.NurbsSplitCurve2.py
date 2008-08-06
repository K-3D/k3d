#python

import testing
import k3d

document = k3d.new_document()
setup = testing.setup_mesh_reader_test("K3DMeshReader","mesh.modifier.NurbsCurveInsertKnotReference.k3d")


modifier = setup.document.new_node("NurbsSplitCurve")
modifier.u_value = 0.321

document.set_dependency(modifier.get_property("input_mesh"), setup.reader.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsSplitCurve2", 1)

