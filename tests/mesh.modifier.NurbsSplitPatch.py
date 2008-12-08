#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsGrid","NurbsSplitPatch")

selection = k3d.mesh_selection.deselect_all()
selection.nurbs_patches = k3d.mesh_selection.component_select_all()

setup.modifier.mesh_selection = selection
setup.modifier.u_value = 0.567
setup.modifier.insert_to_v = False

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsSplitPatch1", 2)

