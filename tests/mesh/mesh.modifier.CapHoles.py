#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyText", "CapHoles")

selection = k3d.geometry.selection.create(0)
edge_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.EDGE)
k3d.geometry.primitive_selection.append(edge_selection, 48, 49, 1)

setup.source.text = "8"
setup.modifier.mesh_selection = selection

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CapHoles", 2, ["Darwin-i386"])

