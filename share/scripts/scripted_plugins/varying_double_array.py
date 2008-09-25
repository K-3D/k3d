#python

# k3d:plugin-class="document"
# k3d:plugin-type="MeshModifierScript"
# k3d:plugin-name="VaryingDoubleArray"
# k3d:plugin-description="Adds an array of facevarying double data to the input mesh"

import k3d
k3d.check_node_environment(locals(), "MeshModifierScript")

Output.copy(Input)

if Output.polyhedra() and Output.polyhedra().edge_points():
  D = Output.writable_polyhedra().writable_face_varying_data().create_array("D", "k3d::double_t")
  for i in range(len(Output.polyhedra().edge_points())):
    D.append(i)