#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCylinder", "CatmullClark")
setup.source.u_segments = 5
setup.source.v_segments = 2
setup.source.bottom = False

setup.modifier.mesh_selection = k3d.mesh_selection.select_all()

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CatmullClark", 1)

