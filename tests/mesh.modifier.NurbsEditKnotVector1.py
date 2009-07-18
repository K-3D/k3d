#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsCurve","NurbsEditCurveKnotVector")

mesh_selection = k3d.geometry.selection.create(0)
uniform_selection = k3d.geometry.primitive_selection.create(mesh_selection, k3d.selection.type.UNIFORM)
k3d.geometry.primitive_selection.append(uniform_selection, 1)

setup.modifier.mesh_selection = mesh_selection
knot_vector = [0.0,0.0,1.0,1.0,2.0,2.0,2.0]

setup.modifier.knot_vector = knot_vector

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsEditKnotVector", 1)

