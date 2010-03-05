#python
# Sample RenderEngineScript input
#
# Use the following context variables for rendering:
#
# "context.document" - a reference to the owning document.
# "context.node" - a reference to the owning node.
# "context.visible_nodes" - the collection of nodes that should be
#                           rendered, if possible.
# "context.output_image" - path to the user-selected output file.
# "context.view_image" - boolean indicating whether the output should
#                        be displayed after rendering is complete.

# This trivial example "renders" the document by writing
# the name of each visible node to a text file.  The set of
# visible nodes is chosen by the user at runtime via the
# "Visible Nodes" property.

import k3d

k3d.check_node_environment(locals(), "RenderEngineScript")

output = open(str(context.output_image), "w")

for node in context.visible_nodes:
	output.write(node.name + "\n")

output.close()

