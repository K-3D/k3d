#python

import k3d

class inspector:
	def print_node(self, Node, Level):
		message = ""
		for j in range(Level):
			message += "    "

		message += "command-node: " + Node.command_node_name()

		k3d.log.debug(message)

		for child in Node.children():
			self.print_node(child, Level + 1)

for node in k3d.command_nodes():
	inspector().print_node(node, 0)

k3d.ui().message("Output sent to the K-3D log window.")

