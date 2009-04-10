#python

import testing
import k3d

document = k3d.new_document()
setup = testing.setup_mesh_modifier_test("NurbsCircle","NurbsCreateCap")

selection = k3d.mesh_selection.select_all()
setup.modifier.mesh_selection = selection

testing.mesh_comparison_to_reference(document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsCreateCap", 1)

