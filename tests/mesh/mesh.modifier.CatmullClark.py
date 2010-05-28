#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "CatmullClark")
testing.add_point_attributes_test(setup, True, False, False)

setup.source.u_segments = 4
setup.source.v_segments = 2
setup.source.bottom = False

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
#testing.require_valid_point_attributes(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CatmullClark", 2)

