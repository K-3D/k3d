#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "CatmullClark")
testing.add_point_attributes_test(setup, True, False, False)

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
#testing.require_valid_point_attributes(setup.document, setup.modifier.output_mesh)
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CatmullClark.cube", 2)

