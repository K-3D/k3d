#python

import k3d
import testing
import math

setup = testing.setup_mesh_modifier_test("PolyCube", "CalculateNormals")
setup.modifier.mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.max_angle = math.radians(91.0)
setup.modifier.face = True
setup.modifier.varying = True
setup.modifier.vertex = True
setup.modifier.face_array = "Nu"
setup.modifier.varying_array = "Nf"
setup.modifier.vertex_array = "Nv"

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CalculateNormals.2", 1)

