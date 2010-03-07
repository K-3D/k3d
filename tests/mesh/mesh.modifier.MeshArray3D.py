#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "MeshArray3D")

setup.modifier.layout = k3d.plugin.create("TranslateArray3D", setup.document)

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.MeshArray3D", 1)

