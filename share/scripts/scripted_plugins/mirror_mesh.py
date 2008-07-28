#python
# k3d:plugin-class="application"
# k3d:plugin-type="ActionScript"
# k3d:plugin-name="Mirror and Weld"
# ngui:action=""

import k3d

class MirrorMesh:
	"""
		This class implements functions to Mirror selected Instances.
		At the construction time the Mirror objects are created, there is no need to call a method later.
		(actually is just like calling a function)
		Usage:
		MirrorMesh(document, mesh_instance, axis="x", new_node="reuse", new_mesh_instance_sufix=" plus Mirror")
		There are two ways of mirroring:
	 		-reusing the same instance (thus eliminating the option of
			keep modifying the original half of object) Example:
				temp_unnecessary_object = MirrorMesh(Document, node, "reuse")
			-creating parallel instance withe the name <Original mesh instance name>+<new_mesh_instance_sufix>
			Example:
				MirrorMesh(Document, node, "new", "Mirror")
				#where "Mirror" is the sufix of the new created instances
	"""
	def __init__(self, document, mesh_instance, axis="x", new_node="reuse", new_mesh_instance_sufix="Mirror and Weld"):
		self.doc = document
		self.mesh_instance = mesh_instance
		if new_node ==  "new":
			self.new_mesh_instance_sufix = new_mesh_instance_sufix
			self.create_new_mesh_instance()
		self.axis = axis
		
		self.create_nodes()
		self.mesh_instance_child = self.get_child(self.mesh_instance)
		self.set_dependencies()
		self.set_mirror_matrix()
		self.set_modifiers_selection()
	def create_new_mesh_instance(self):
		self.old_mesh_instance = self.mesh_instance
		self.mesh_instance = self.doc.new_node("MeshInstance")
		self.mesh_instance.name = self.old_mesh_instance.name +" "+ self.new_mesh_instance_sufix
		self.mesh_instance.gl_painter = self.get_node("GL Default Painter")
		self.mesh_instance.ri_painter = self.get_node("RenderMan Default Painter")
		self.connect_nodes("mesh_instance","input_mesh","old_mesh_instance","output_mesh")
	def create_nodes(self):
		self.create_node("Weld", "weld", "Weld", "Weld Points Modifier")
		self.create_node("MergeMesh", "merge_mesh", "MergeMesh", "MergeMesh Modifier")
		self.create_merge_mesh_inputs()
		self.create_node("TransformPoints", "transform_points", "Transformation Mirror", "TransformPoints Modifier")
		self.create_node("FrozenTransformation", "frozen_matrix", "FrozenTransformation", "Matrix for the TransformPoints Modifier")
		self.create_node("FlipOrientation", "flip_orientation", "FlipOrientation", "FlipOrientation of the TransformPoints output")
	def create_node(self, type_name, property_name, label, description):
		node = self.doc.new_node(type_name)
		exec("self."+ property_name + " = node")
	def create_merge_mesh_inputs(self):
		self.merge_mesh.create_property("k3d::mesh*", "input_mesh", "Input Mesh", "")
		self.merge_mesh.create_property("k3d::mesh*", "input_mesh1", "Input Mesh 1", "")
	def set_dependencies(self):
		self.connect_nodes("transform_points","input_matrix","frozen_matrix","output_matrix")
		self.connect_nodes("transform_points","input_mesh","mesh_instance_child","output_mesh")
		self.connect_nodes("flip_orientation","input_mesh","transform_points","output_mesh")
		self.connect_nodes("merge_mesh","input_mesh","flip_orientation","output_mesh")
		self.connect_nodes("merge_mesh","input_mesh1","mesh_instance_child","output_mesh")
		self.connect_nodes("weld","input_mesh","merge_mesh","output_mesh")
		self.connect_nodes("mesh_instance","input_mesh","weld","output_mesh")
	def connect_nodes(self, name1, property1, name2, property2):
		exec("node1 = self."+name1)
		exec("node2 = self."+name2)
		self.doc.set_dependency(node1.get_property(property1), node2.get_property(property2))
	def set_mirror_matrix(self):
		matrix = self.frozen_matrix.matrix
		x = y = z = 1
		if self.axis.lower() == "x":
			x=-x
		elif self.axis.lower() == "y":
			y=-y
		else:
			z=-z
		row = matrix[0]; row[0] = x; matrix[0] = row
		row = matrix[1]; row[1] = y; matrix[1] = row
		row = matrix[2]; row[2] = z; matrix[2] = row
		self.frozen_matrix.matrix = matrix
	def set_modifiers_selection(self):
		selection = k3d.select_all()
		#selection.points = k3d.component_select_all()
		self.transform_points.mesh_selection = selection
		selection = k3d.select_all()
		self.flip_orientation.mesh_selection = selection
	def get_child(self, node):
		return self.doc.get_dependency(node.get_property("input_mesh")).node()
	def get_factory_id(self, name):
		temp_node = self.doc.new_node(name)
		id_node_factory = temp_node.factory().factory_id()
		self.doc.delete_node(temp_node)
		return id_node_factory
	def get_node(self, name):
		node = self.doc.get_node(name)
		if hasattr(node, "name"):
			return node
		return None

def get_factory_id( name):
	temp_node = Document.new_node(name)
	id_node_factory = temp_node.factory().factory_id()
	Document.delete_node(temp_node)
	return id_node_factory

chosen_axis = k3d.ui().query_message("Choose the mirroring Axis:", ["X", "Y", "Z"])
keep_original = k3d.ui().query_message("Keep the Original Instance?", ["No", "Yes"])

if chosen_axis == 1:
	axis = "x"
elif chosen_axis == 2:
	axis = "y"
else:
	axis = "z"

if keep_original == 1: #dont keep
	creation_mode = "reuse"
else: #keep
	creation_mode = "new"

Document.start_change_set()
try: 
	selection_node = Document.get_node_by_metadata("inode_selection", "ngui:unique_node", "node_selection")
	id_msh_inst = get_factory_id("MeshInstance")
	for node in Document.nodes():
		if id_msh_inst == node.factory().factory_id() and selection_node.selection_weight(node):
			MirrorMesh(Document, node, axis, creation_mode)
	Document.finish_change_set("Mirror Selected Mesh Instances")
except: #there was an error
	Document.cancel_change_set()
	raise
