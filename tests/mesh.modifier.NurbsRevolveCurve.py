#python

import testing
import k3d

document = k3d.new_document()
setup = testing.setup_mesh_reader_test("K3DMeshReader","mesh.modifier.NurbsRevolveCurveReference.k3d")

modifier = setup.document.new_node("NurbsRevolveCurve")
modifier.create_caps = True
modifier.around = 'z'
modifier.delete_original = True

modifier.mesh_selection = k3d.mesh_selection.select_all()

document.set_dependency(modifier.get_property("input_mesh"), setup.reader.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsRevolveCurve", 1)

