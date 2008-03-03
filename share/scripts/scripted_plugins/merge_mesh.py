#python

# k3d:plugin-class="document"
# k3d:plugin-type="MeshSourceScript"
# k3d:plugin-name="MergeMeshScript"
# k3d:plugin-description="Merge meshes given to user input properties"

import k3d

# extends array Output using the elements of Input increased by Offset
def extend_array(Input, Output, Offset = 0):
  if Offset == 0:
    for i in range(len(Input)):
      Output.append(Input[i])
  else:
    for i in range(len(Input)):
      Output.append(Input[i] + Offset)

points = Output.create_points()
point_selection = Output.create_point_selection()
polyhedra = Output.create_polyhedra()
first_faces = polyhedra.create_first_faces()
face_counts = polyhedra.create_face_counts()
types = polyhedra.create_types()
face_first_loops = polyhedra.create_face_first_loops()
face_loop_counts = polyhedra.create_face_loop_counts()
face_selection = polyhedra.create_face_selection()
face_materials = polyhedra.create_face_materials()
loop_first_edges = polyhedra.create_loop_first_edges()
edge_points = polyhedra.create_edge_points()
clockwise_edges = polyhedra.create_clockwise_edges()
edge_selection = polyhedra.create_edge_selection()

for property in Node.properties():
  if property.type() == "k3d::mesh*" and property.name() != "output_mesh":
    input_mesh = property.pipeline_value()
    # extend point selection
    extend_array(input_mesh.point_selection(), point_selection, len(points))
    # get input polyhedra and components
    in_polyhedra = input_mesh.polyhedra()
    # extend polyhedra components
    extend_array(in_polyhedra.first_faces(), first_faces, len(face_loop_counts))
    extend_array(in_polyhedra.face_counts(), face_counts)
    extend_array(in_polyhedra.types(), types)
    extend_array(in_polyhedra.face_selection(), face_selection, len(face_first_loops))
    extend_array(in_polyhedra.face_first_loops(), face_first_loops, len(loop_first_edges))
    extend_array(in_polyhedra.face_loop_counts(), face_loop_counts)
    extend_array(in_polyhedra.face_materials(), face_materials)
    extend_array(in_polyhedra.edge_selection(), edge_selection, len(edge_points))
    extend_array(in_polyhedra.loop_first_edges(), loop_first_edges, len(edge_points))
    extend_array(in_polyhedra.clockwise_edges(), clockwise_edges, len(edge_points))
    extend_array(in_polyhedra.edge_points(), edge_points, len(points))
    # extend points
    extend_array(input_mesh.points(), points)
