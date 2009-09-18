#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsCurve","NurbsCurveDegreeElevation")

mesh_selection = k3d.geometry.selection.create(0)
uniform_selection = k3d.geometry.primitive_selection.create(mesh_selection, k3d.selection.type.CURVE)
k3d.geometry.primitive_selection.append(uniform_selection, 1)

setup.modifier.mesh_selection = mesh_selection
setup.modifier.degree = 1

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsDegreeElevateCurve", 1)

