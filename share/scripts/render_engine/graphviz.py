#python

import k3d
k3d.check_node_environment(locals(), "RenderEngineScript")

graph = """
digraph G {

node [ shape="box" style="filled" color="black" fillcolor="white" ];

"""

for node in Document.nodes():
	graph += str(node.__hash__())
	graph += "[\n"
	graph += "label=\"" + node.name + "\"\n"
	graph += "URL=\"http://www.k-3d.org/wiki/" + node.factory().name + "\"\n"
	graph += "]\n"

for node in Document.nodes():
	for prop in node.properties():

		if prop.type == "k3d::inode*":
			referenced_node = prop.internal_value
			if referenced_node:
				graph += str(referenced_node.__hash__()) + "->" + str(node.__hash__())
				graph += "[\n"
				graph += "style=dotted\n"
				graph += "label=\"" + prop.name + "\"\n"
				graph += "]\n"

		source_prop = Document.get_dependency(prop)
		if source_prop:
			graph += str(source_prop.node.__hash__()) + "->" + str(prop.node.__hash__())
			graph += "[\n"
			graph += "taillabel=\"" + source_prop.name + "\"\n"
			graph += "headlabel=\"" + prop.name + "\"\n"
			graph += "]\n"

graph += "}"


from subprocess import *
Popen(["dot", "-Tsvg", "-o" + OutputImage], stdin=PIPE).communicate(graph)
