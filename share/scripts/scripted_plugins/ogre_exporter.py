#python

# k3d:plugin-class="document"
# k3d:plugin-type="NullOutputScript"
# k3d:plugin-name="OGREWriter"
# k3d:plugin-description="Exports a mesh to OGRE XML format"

from xml.dom.minidom import getDOMImplementation
import k3d

#Add the required user properties
if not Node.has_property("input_mesh"):
	Node.add_user_property("k3d::mesh*", "input_mesh", "Input Mesh", "The mesh that will have its points counted")
if not Node.has_property("output_file"):
	Node.add_user_property("k3d::filesystem::path", "output_file", "Output File", "The exportes OGRE mesh XML file")

# mesh components
points =  Node.__getattr__("input_mesh").points()
polyhedra = Node.__getattr__("input_mesh").polyhedra()
edge_points = polyhedra.edge_points()
clockwise_edges = polyhedra.clockwise_edges()
face_first_loops = polyhedra.face_first_loops()
loop_first_edges = polyhedra.loop_first_edges()

#init xml file
impl = getDOMImplementation()
doc = impl.createDocument(None, "mesh", None)
top_element = doc.documentElement
sharedgeometry = doc.createElement("sharedgeometry")
sharedgeometry.setAttribute("vertexcount", str(len(points)))
vertexbuffer = doc.createElement("vertexbuffer")
vertexbuffer.setAttribute("positions", "true")

# append points
for i in range(len(points)):
	vertex = doc.createElement("vertex")
	position = doc.createElement("position")
	position.setAttribute("x", str(points[i][0]))
	position.setAttribute("y", str(points[i][1]))
	position.setAttribute("z", str(points[i][2]))
	vertex.appendChild(position)
	vertexbuffer.appendChild(vertex)

# finalize sharedgeometry element
sharedgeometry.appendChild(vertexbuffer)
top_element.appendChild(sharedgeometry)

#init submeshes
submeshes = doc.createElement("submeshes")
submesh = doc.createElement("submesh")
faces = doc.createElement("faces")
faces.setAttribute("count", str(len(face_first_loops)))

#Check if mesh is triangles-only
if not k3d.is_triangles(Node.__getattr__("input_mesh")):
	raise Exception, "Mesh has non-triangular faces"

#Loop over all faces, and add them as triangles
for face in range(len(face_first_loops)):
	first_edge = loop_first_edges[face_first_loops[face]]
	edge = first_edge
	face_element = doc.createElement("face")
	for n in range(1, 4):
		face_element.setAttribute("v" + str(n), str(edge_points[edge]))
		edge = clockwise_edges[edge]
	faces.appendChild(face_element)

#append the faces and submesh block to the main document
submesh.appendChild(faces)
submeshes.appendChild(submesh)
top_element.appendChild(submeshes)

# write xml file
f = open(str(Node.__getattr__("output_file")), 'w')
doc.writexml(f)
f.close()

