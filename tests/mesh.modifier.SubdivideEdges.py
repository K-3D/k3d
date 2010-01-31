#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("K3DMeshReader", "SubdivideEdges")
setup.source.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/polyhedron.hole.k3d")
setup.source.center = False
setup.source.scale_to_size = False

selection = k3d.geometry.selection.create(0)
edge_selection = k3d.geometry.primitive_selection.create(selection, k3d.selection.type.EDGE)
k3d.geometry.primitive_selection.append(edge_selection, 0, 4294967295, 0)
k3d.geometry.primitive_selection.append(edge_selection, 0, 4, 1)
k3d.geometry.primitive_selection.append(edge_selection, 27, 28, 1)
k3d.geometry.primitive_selection.append(edge_selection, 52, 65, 1)
k3d.geometry.primitive_selection.append(edge_selection, 74, 82, 1)
setup.modifier.mesh_selection = selection
setup.modifier.vertices = 2

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.SubdivideEdges", 1)
