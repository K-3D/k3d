#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsGrid","NurbsSplitPatch")
#testing.add_point_attributes_test(setup, True, True, True)

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.u_value = 0.567
setup.modifier.insert_to_v = False

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
#testing.require_valid_point_attributes(setup.document, setup.modifier.output_mesh)
#testing.require_valid_vertex_attributes(setup.document, setup.modifier.output_mesh)
#testing.require_valid_parameter_attributes(setup.document, setup.modifier.output_mesh)
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsSplitPatch1", 2)

