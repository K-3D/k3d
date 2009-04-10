#python

import testing
import k3d

document = k3d.new_document()
setup = testing.setup_mesh_modifier_test("NurbsCircle","NurbsSetWeight")

modifier = setup.modifier
modifier.weight = 1.7
modifier.mesh_selection = k3d.mesh_selection.select_all()

testing.mesh_comparison_to_reference(document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsSetWeight1", 1)

