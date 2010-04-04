#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsCurve","NurbsSplitCurve")
#testing.add_point_attributes_test(setup, True, True, True)

selection = k3d.geometry.selection.create(0)
curve_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.CURVE)
k3d.geometry.primitive_selection.append(curve_selection, 0, 10000, 1)
setup.modifier.mesh_selection = selection

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
#testing.require_valid_point_attributes(setup.document, setup.modifier.output_mesh)
#testing.require_valid_vertex_attributes(setup.document, setup.modifier.output_mesh)
#testing.require_valid_parameter_attributes(setup.document, setup.modifier.output_mesh)
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsSplitCurve", 1)

