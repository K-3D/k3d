#python

#name: PointCounter
#category: Demo

#Small demo script to demonstrate scripted node creation

#Add the required user properties
if not Node.has_property("input_mesh"):
	Node.add_user_property("k3d::mesh*", "input_mesh", "Input Mesh", "The mesh that will have its points counted")
if not Node.has_property("point_count"):
	Node.add_user_property("k3d::double_t", "point_count", "Point Count", "Point count for the mesh")
	
Node.__setattr__("point_count", len(Node.__getattr__("input_mesh").points()))
