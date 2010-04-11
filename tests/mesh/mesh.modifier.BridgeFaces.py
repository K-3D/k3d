#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("K3DMeshReader", "BridgeFaces")
setup.source.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/mesh.modifier.BridgeFaces.source.k3d")

selection = k3d.geometry.selection.create(0)
edge_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.FACE)
k3d.geometry.primitive_selection.append(edge_selection, 8, 9, 1)
k3d.geometry.primitive_selection.append(edge_selection, 12, 13, 1)

setup.modifier.mesh_selection = selection

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.BridgeFaces", 1)

