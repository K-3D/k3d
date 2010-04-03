#python

import testing
import k3d

setup = testing.setup_mesh_modifier_test("NurbsCircle", "NurbsCurveInsertKnot")

setup.modifier.u_value = 0.1

selection = k3d.geometry.selection.create(0)
curve_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.CURVE)
k3d.geometry.primitive_selection.append(curve_selection, 0, 10000, 1)
setup.modifier.mesh_selection = selection

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsCurveInsertKnot", 2, ["Darwin-i386"])

