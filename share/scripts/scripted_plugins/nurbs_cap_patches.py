#python

# k3d:plugin-class="application"
# k3d:plugin-type="ActionScript"
# k3d:plugin-name="NurbsCapPatches"
# ngui:action=""

import k3d

# Inserts a NurbsExtractPatchCurve node before the given MeshInstance
def extract_curve(Node, U, UDirection):
	modifier = Document.new_node('NurbsExtractPatchCurve')
	if len(Node.mesh_selection) == 0:
		modifier.mesh_selection = k3d.geometry.selection.create(1)
	else:
		modifier.mesh_selection = Node.mesh_selection
	modifier.parameter = U
	modifier.extract_v_curve = not UDirection
	source = Document.get_dependency(Node.get_property('input_mesh')).node()
	Document.set_dependency(modifier.get_property('input_mesh'), source.get_property('output_mesh'))
	Document.set_dependency(Node.get_property('input_mesh'), modifier.get_property('output_mesh'))

# Inserts a NurbsCreateCap node before the given MeshInstance
def cap_curves(Node, Coons):
	modifier = Document.new_node('NurbsCreateCap')
	modifier.mesh_selection = k3d.geometry.selection.create(1)
	modifier.coons = Coons
	source = Document.get_dependency(Node.get_property('input_mesh')).node()
	Document.set_dependency(modifier.get_property('input_mesh'), source.get_property('output_mesh'))
	Document.set_dependency(Node.get_property('input_mesh'), modifier.get_property('output_mesh'))

u_direction = True

try:
	Document.start_change_set()
	selection_node = Document.get_node_by_metadata("ngui:unique_node", "node_selection")
	selected_nodes = selection_node.selected_nodes()
	for node in selected_nodes:
		if node.factory().name() == 'MeshInstance':
			has_nurbs_patch = False
			prims = node.output_mesh.primitives()
			for prim in prims:
				if prim.type() == 'nurbs_patch':
					has_nurbs_patch = True
			if has_nurbs_patch:
				extract_curve(node, 0, u_direction)
				extract_curve(node, 1, u_direction)
				cap_curves(node, True)
	Document.finish_change_set("NurbsCapPatches")

except:
	Document.cancel_change_set()
