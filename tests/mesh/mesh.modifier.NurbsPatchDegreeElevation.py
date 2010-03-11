#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsGrid","NurbsPatchDegreeElevation")

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.degree = 1
setup.modifier.insert_to_v=False


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsPatchDegreeElevation", 2)

