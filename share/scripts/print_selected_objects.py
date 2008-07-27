#python

import k3d

selection_node = Document.get_node_by_metadata("inode_selection", "ngui:unique_node", "node_selection")

for node in selection_node.selected_nodes():
		print "Selected:", node.name

import sys
sys.stdout.flush()

k3d.ui().message("Output sent to console")

