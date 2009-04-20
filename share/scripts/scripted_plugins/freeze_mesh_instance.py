#python
# k3d:plugin-class="application"
# k3d:plugin-type="ActionScript"
# k3d:plugin-name="Freeze Mesh Instance"
# ngui:action=""

import k3d

class FreezeMeshInstance:
	"""
		This class implements functions to Freeze Mesh Instances.
		At the construction of the object the freeze is done, there is no need to call a method later.
		(actually is just like calling a function
		Usage:
		FreezeMeshInstance(document, freeze_mode="reuse", freeze_scope="selected")
		-The freeze_mode parameter means if the freezon object is a new Mesh Instance or it reuses the mesh instance
		of the original object, thus deleting all the pipeline down.
		-freeze_scope says if we are going to freeze the whole document or just selected nodes
	"""
	def __init__(self, document, freeze_mode="reuse", freeze_scope="selected"):
		self.doc = document
		self.created_nodes = []
		self.orphans = []
		self.freeze_mode = freeze_mode
		self.freeze_instances(freeze_scope)
		
	def freeze_instances(self,mode="all"):
		id_msh_inst = self.get_factory_id("MeshInstance")
		selection_node = Document.get_node_by_metadata("ngui:unique_node", "node_selection")
		if(mode == "all"):
			for node in self.doc.nodes():
				if id_msh_inst == node.factory().factory_id():
					self.freeze_instance(node)
		elif(mode == "selected"):
			for node in self.doc.nodes():
				if id_msh_inst == node.factory().factory_id() and selection_node.selection_weight(node):
					self.freeze_instance(node) #There is an instance selected
		self.delete_garbage()
			
	def freeze_instance(self,node): #choose mode method
		if(self.freeze_mode == "reuse"):
			self.freeze_reusing_instance(node)
		elif(self.freeze_mode == "new"):
			self.freeze_new_instance(node)
		
	def freeze_new_instance(self, node):
		frozen_mesh = self.doc.new_node("FrozenMesh")
		frozen_mesh.name = "Frozen "+node.name
		self.doc.set_dependency(frozen_mesh.get_property("input_mesh"), node.get_property("output_mesh"))
		temp_mesh  = frozen_mesh.output_mesh
		self.doc.set_dependency(frozen_mesh.get_property("input_mesh"), None)
		
		frzmsh_instance = self.doc.new_node("MeshInstance")
		frzmsh_instance.name = "Frozen Mesh Instance"
		frzmsh_instance.gl_painter = self.doc.get_node("GL Default Painter")
		frzmsh_instance.ri_painter = self.doc.get_node("RenderMan Default Painter")
		self.doc.set_dependency(frzmsh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))
		frzmsh_instance.input_matrix = node.output_matrix
		
		self.created_nodes.append(frozen_mesh.__hash__())
		self.created_nodes.append(frzmsh_instance.__hash__())
		
	def freeze_reusing_instance(self, node):
		frozen_mesh = self.doc.new_node("FrozenMesh")
		frozen_mesh.name = "Frozen "+node.name
		self.doc.set_dependency(frozen_mesh.get_property("input_mesh"), node.get_property("output_mesh"))
		temp_mesh  = frozen_mesh.output_mesh
		self.doc.set_dependency(frozen_mesh.get_property("input_mesh"), None) 
		
		self.orphans.append(self.get_child(node).__hash__())
		
		self.doc.set_dependency(node.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))
		
	def get_factory_id(self, name):
		temp_msh_inst = self.doc.new_node(name)
		id_msh_inst = temp_msh_inst.factory().factory_id()
		self.doc.delete_node(temp_msh_inst)
		return id_msh_inst
		
	def delete_all_the_rest(self):
		for node in self.doc.nodes():
			try:
				# Did we made this node?
				self.created_nodes.index(node.__hash__())
			except:
				#Not on the list
				self.doc.delete_node(node)
		
	def delete_orphans(self):
		for node in self.doc.nodes():
			try:
				# Is an orphan?
				self.orphans.index(node.__hash__())
				self.delete_downstream(node)
			except:
				#Not on the list
				pass
		
	def delete_garbage(self):
		if(self.freeze_mode == "reuse"):
			self.delete_orphans()
		elif(self.freeze_mode == "new"):
			#self.delete_all_the_rest()
			pass
		else:
			pass
		
	def get_child(self, node):
		return self.doc.get_dependency(node.get_property("input_mesh")).node()
		
	def delete_downstream(self, node):
		while( node != None ):
			try:
				node_tmp = self.get_child(node,doc)
				self.doc.delete_node(node)
				node = node_tmp
			except:
				self.doc.delete_node(node)
				node = None
		

freeze_scope = k3d.ui().query_message("Freeze Whole Document or Selected Instances?:", ["Whole", "Selected"])
keep_original = k3d.ui().query_message("Keep the Original Instances?", ["No", "Yes"])

if freeze_scope == 1:
	freeze_scope_string = "all"
else:
	freeze_scope_string = "selected"

if keep_original == 1: #dont keep
	creation_mode = "reuse"
else: #keep
	creation_mode = "new"

Document.start_change_set()
try:
	FreezeMeshInstance(Document, creation_mode, freeze_scope_string)
	Document.finish_change_set("Freeze Mesh Instance")
except: #there was an error
	Document.cancel_change_set()
	raise
