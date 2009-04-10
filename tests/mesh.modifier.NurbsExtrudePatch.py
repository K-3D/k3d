#python

import testing
import k3d

document = k3d.new_document()
setup = testing.setup_mesh_modifier_test("NurbsDisk", "NurbsExtrudePatch")
setup.modifier.mesh_selection = k3d.mesh_selection.select_all()

testing.mesh_comparison_to_reference(document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsExtrudePatch", 1)

