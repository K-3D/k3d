#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "CalculateNormals")
setup.modifier.uniform = True
setup.modifier.face_varying = True
setup.modifier.vertex = True
setup.modifier.uniform_array = "Nu"
setup.modifier.face_varying_array = "Nf"
setup.modifier.vertex_array = "Nv"

testing.mesh_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CalculateNormals", 1)

