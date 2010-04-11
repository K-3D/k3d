#python

import k3d

k3d.log.debug("***** Selected Nodes *****")

selection_nodes = k3d.node.lookup(context.document, "ngui:unique_node", "node_selection")
if len(selection_nodes):
	for node in selection_nodes[0].selected_nodes():
		k3d.log.debug(node.name)
k3d.log.debug("**************************")

k3d.ui().message("Output sent to the K-3D log ... check your console output, or use Help > Open Log Window.")

