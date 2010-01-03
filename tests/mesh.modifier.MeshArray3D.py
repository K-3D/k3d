#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "MeshArray3D")

setup.modifier.layout = setup.document.new_node("TranslateArray3D")


testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.MeshArray3D", 1)

