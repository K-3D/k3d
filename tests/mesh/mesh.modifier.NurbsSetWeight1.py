#python

import testing
import k3d

setup = testing.setup_mesh_modifier_test("NurbsCircle","NurbsSetWeight")

modifier = setup.modifier
modifier.weight = 1.7
modifier.mesh_selection = k3d.geometry.selection.create(1)

testing.require_valid_mesh(setup.document, modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, modifier.get_property("output_mesh"), "mesh.modifier.NurbsSetWeight1", 1, ["Darwin-i386"])

