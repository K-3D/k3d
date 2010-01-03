#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "MakeParticles")

testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.MakeParticles", 1)

