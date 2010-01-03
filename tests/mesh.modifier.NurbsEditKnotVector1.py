#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsCircle","NurbsEditCurveKnotVector")

selection = k3d.geometry.selection.create(0)
curve_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.CURVE)
k3d.geometry.primitive_selection.append(curve_selection, 0, 10000, 1)
setup.modifier.mesh_selection = selection

setup.modifier.knot_vector = [0, 0, 0, 0.1, 0.2, 0.25, 0.3, 0.3, 0.8, 0.8, 1, 1, 1]


testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsEditKnotVector", 1)

