#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("K3DMeshReader", "CUDASubdivideEdges")
setup.source.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/polyhedron.k3d")

# select some edges, distributed along polyhedra
selection = k3d.geometry.selection.create(0)
selection.edges =[(0, 4294967295, 0), (0, 1, 1), (1, 2, 1), (2, 3, 1), (3, 4, 1), (52, 53, 1), (53, 54, 1), (54, 55, 1), (55, 56, 1), (56, 57, 1), (57, 58, 1), (58, 59, 1), (59, 60, 1), (60, 61, 1), (61, 62, 1), (62, 63, 1), (63, 64, 1), (64, 65, 1), (74, 75, 1), (75, 76, 1), (76, 77, 1), (77, 78, 1), (78, 79, 1), (79, 80, 1), (80, 81, 1), (81, 82, 1)]
setup.modifier.mesh_selection = selection
setup.modifier.vertices = 2

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.SubdivideEdges", 1)

