#python

import testing
import k3d

setup = testing.setup_mesh_modifier_test("NurbsCircle", "NurbsCloseCurve")

selection = k3d.geometry.selection.create(0)
curve_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.CURVE)
k3d.geometry.primitive_selection.append(curve_selection, 0, 10000, 1)
setup.modifier.mesh_selection = selection

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsCloseCurve2", 32)