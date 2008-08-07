#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsCurve","NurbsEditCurveKnotVector")

selection = k3d.deselect_all()
selection.nurbs_curves = k3d.component_select_all()

setup.modifier.mesh_selection = selection
knot_vector = [0.0,0.0,1.0,1.0,2.0,2.0,2.0]

setup.modifier.knot_vector = knot_vector

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsEditKnotVector", 1)

