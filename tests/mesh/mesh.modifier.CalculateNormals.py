#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "CalculateNormals")
setup.modifier.mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.face = True
setup.modifier.vertex = True
setup.modifier.point = True
setup.modifier.face_array = "Nf"
setup.modifier.vertex_array = "Nv"
setup.modifier.point_array = "Np"


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CalculateNormals", 1)

