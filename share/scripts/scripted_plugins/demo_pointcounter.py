#python

# k3d:plugin-class="document"
# k3d:plugin-type="NullOutputScript"
# k3d:plugin-name="PointCounter"
# k3d:plugin-description="Sample plugin script that counts the points in its input mesh"

#Small demo script to demonstrate scripted node creation

#Add the required user properties
if not hasattr(Node, "input_mesh"):
	Node.create_property("k3d::mesh*", "input_mesh", "Input Mesh", "The mesh that will have its points counted")
if not hasattr(Node, "point_count"):
	Node.create_property("k3d::double_t", "point_count", "Point Count", "Point count for the mesh")

mesh = Node.input_mesh
if mesh:
	Node.point_count = len(mesh.points())
else:
	Node.point_count = 0

