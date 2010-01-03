#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsCurve","NurbsSplitCurve")

selection = k3d.geometry.selection.create(0)
curve_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.CURVE)
k3d.geometry.primitive_selection.append(curve_selection, 0, 10000, 1)
setup.modifier.mesh_selection = selection

testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsSplitCurve", 1)

