#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

# Create two triangles, arranged to form a square ...
vertices = [k3d.point3(-3, -3, 0), k3d.point3(3, -3, 0), k3d.point3(3, 3, 0), k3d.point3(-3, 3, 0)]
vertex_counts = [3, 3]
vertex_indices = [0, 1, 3, 1, 2, 3]
material = None
polyhedron = k3d.polyhedron.create(Output, vertices, vertex_counts, vertex_indices, material)

