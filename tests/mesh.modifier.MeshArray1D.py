#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "MeshArray1D")

setup.modifier.layout = setup.document.new_node("TranslateArray1D")

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.MeshArray1D", 1)

