#python

# k3d:plugin-class="document"
# k3d:plugin-type="NullOutputScript"
# k3d:plugin-name="AC3DWriter"
# k3d:plugin-description="Exports a mesh to AC3D format"

import k3d

#Add the required user properties
if not Node.has_property("input_mesh"):
  Node.create_property("k3d::mesh*", "input_mesh", "Input Mesh", "The mesh that will have its points counted")
if not Node.has_property("output_file"):
  Node.create_property("k3d::filesystem::path", "output_file", "Output File", "The exported AC3D mesh file")

# mesh components
points =  Node.__getattr__("input_mesh").points()
polyhedra = Node.__getattr__("input_mesh").polyhedra()
edge_points = polyhedra.edge_points()
clockwise_edges = polyhedra.clockwise_edges()
face_first_loops = polyhedra.face_first_loops()
loop_first_edges = polyhedra.loop_first_edges()

# open the file
f = open(str(Node.__getattr__("output_file")), 'w')

#header
f.write("AC3Db\n")

#set a default material (todo: use K-3D assigned materials)
f.write("MATERIAL \"ac3dmat1\" rgb 1 1 1  amb 0.2 0.2 0.2  emis 0 0 0  spec 0.5 0.5 0.5  shi 10  trans 0\n")

#Create a world object with one child
f.write("OBJECT world\n")
f.write("kids 1\n")
f.write("OBJECT poly\n")

# write points array
f.write("numvert " + str(len(points)) + "\n")
for i in range(len(points)):
  f.write(str(points[i][0]) + " " + str(points[i][1]) + " " + str(points[i][2]) + "\n")

# Add faces
f.write("numsurf " + str(len(face_first_loops)) + "\n")
for face in range(len(face_first_loops)):
  f.write("SURF 0x0\n")
  f.write("mat 0\n")
  first_edge = loop_first_edges[face_first_loops[face]]
  edge = first_edge
  facepoints = []
  for n in range(1, len(clockwise_edges)):
    facepoints.append(edge_points[edge])
    edge = clockwise_edges[edge]
    if edge == first_edge:
      break
  f.write("refs " + str(len(facepoints)) + "\n")
  for corner in facepoints:
    f.write(str(corner) + " 0 1\n") # todo: texture coordinates

f.write("kids 0\n")
# commit to file
f.close()

