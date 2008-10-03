#python

# k3d:plugin-class="document"
# k3d:plugin-type="MeshModifierScript"
# k3d:plugin-name="RandomColors"
# k3d:plugin-description="Adds random colors to the attribute arrays in the polyhedra structure and to the vertices"

import k3d
k3d.check_node_environment(locals(), "MeshModifierScript")

from random import seed, randint

Output.copy(Input)

seed(123)

colors = [ k3d.color(1, 0, 0), k3d.color(1, 1, 0), k3d.color(0, 1, 0), k3d.color(0, 1, 1), k3d.color(0, 0, 1), k3d.color(1, 0, 1), k3d.color(1, 1, 1)]

if Output.polyhedra() and Output.polyhedra().edge_points():
  Cs = Output.writable_polyhedra().writable_face_varying_data().create("Cs", "k3d::color")
  for i in range(len(Output.polyhedra().edge_points())):
    Cs.append(colors[i % len(colors)])
  Cs = Output.writable_polyhedra().writable_uniform_data().create("Cs", "k3d::color")
  for i in range(len(Output.polyhedra().face_first_loops())):
    Cs.append(colors[i % len(colors)])
  Cs = Output.writable_polyhedra().writable_constant_data().create("Cs", "k3d::color")
  for i in range(len(Output.polyhedra().first_faces())):
    Cs.append(colors[i % len(colors)])

Cs = Output.writable_vertex_data().create("Cs", "k3d::color")
for point in range(len(Output.points())):
  Cs.append(colors[point % len(colors)])
