#python

import k3d

doc = k3d.new_document()

node_selection = k3d.dynamic_cast(doc.new_node("NodeSelection"), "inode_selection")

node1 = doc.new_node("FrozenMesh")
node2 = doc.new_node("FrozenMesh")
node3 = doc.new_node("FrozenMesh")

node1.name="n1"
node2.name="n2"
node3.name="n3"

node_selection.select(node3, 0.5)
if node_selection.selection_weight(node3) != 0.5:
	raise Exception("incorrect node selection_weight")

node_selection.select(node1, 1.0)
node_selection.select(node2, 1.0)

selected_nodes = node_selection.selected_nodes()

if selected_nodes[0].name != node3.name:
	raise Exception("selection order is wrong")
if selected_nodes[1].name != node1.name:
	raise Exception("selection order is wrong")
if selected_nodes[2].name != node2.name:
	raise Exception("selection order is wrong")

node_selection.select(node1, 0.0)

selected_nodes = node_selection.selected_nodes()

if selected_nodes[0].name != node3.name:
	raise Exception("selection order is wrong")
if selected_nodes[1].name != node2.name:
	raise Exception("selection order is wrong")

node_selection.deselect_all()
selected_nodes = node_selection.selected_nodes()
if len(selected_nodes) != 0:
	raise Exception("deselect_all failed")
