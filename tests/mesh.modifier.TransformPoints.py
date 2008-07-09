#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "TransformPoints")

selection = k3d.deselect_all()
selection.points = k3d.component_select_all()

setup.modifier.mesh_selection = selection
setup.modifier.input_matrix = k3d.translate3(k3d.vector3(0, 0, 1))

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.TransformPoints", 1)

