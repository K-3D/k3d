#python

#name: OGREWriter
#category: MeshWriters

#Add the required user properties
if not Node.has_property("input_mesh"):
	Node.add_user_property("k3d::mesh*", "input_mesh", "Input Mesh", "The mesh that is to be exported")
if not Node.has_property("output_file"):
	Node.add_user_property("k3d::string_t", "output_file", "Output File", "Path to the output OGRE mesh")

