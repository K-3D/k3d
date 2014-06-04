#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test2("PolyCube", "MeshArray2D", "MergePolyhedra")

setup.modifier1.layout = k3d.plugin.create("TranslateArray2D", setup.document)

testing.require_valid_mesh(setup.document, setup.modifier2.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier2.get_property("output_mesh"), "mesh.modifier.MergePolyhedra", 1)

