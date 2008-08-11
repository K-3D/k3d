#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsGrid","NurbsPatchDegreeElevation")

selection = k3d.deselect_all()
selection.nurbs_patches = k3d.component_select_all()

setup.modifier.mesh_selection = selection
setup.modifier.degree = 1

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsPatchDegreeElevation", 1)

