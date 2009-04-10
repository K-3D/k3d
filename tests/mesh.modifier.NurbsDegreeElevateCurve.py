#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsCurve","NurbsCurveDegreeElevation")

selection = k3d.mesh_selection.deselect_all()
component_selection = k3d.mesh_selection.component(0, 1, k3d.selection.type.UNIFORM, 0, 1000, 1)
selection.add_component(component_selection)

setup.modifier.mesh_selection = selection
setup.modifier.degree = 1

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsDegreeElevateCurve", 1)

