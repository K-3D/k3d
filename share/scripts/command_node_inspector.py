#python

import k3d
import sys

class inspector:
	def print_node(self, Node, Level):
		for j in range(Level):
			print " ",

		print "node: " + Node.command_node_name()

		for child in Node.children():
			self.print_node(child, Level + 1)

for node in k3d.command_nodes():
	inspector().print_node(node, 0)

sys.stdout.flush()

k3d.ui.message("Output sent to console")

