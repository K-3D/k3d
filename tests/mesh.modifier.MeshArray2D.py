#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "MeshArray2D")

setup.modifier.layout = setup.document.new_node("TranslateArray2D")

testing.mesh_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.MeshArray2D", 1)

