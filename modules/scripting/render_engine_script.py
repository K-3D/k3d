#python
# Sample RenderEngineScript input
#
# Use the following context variables for rendering:
#
# "Document" - a reference to the owning document.
# "Node" - a reference to the owning node.
# "VisibleNodes" - the collection of nodes that should be
#                  rendered, if possible.
# "OutputImage" - path to the user-selected output file.
# "ViewImage" - boolean indicating whether the output should
#               be displayed after rendering is complete.

# This trivial example "renders" the document by writing
# the name of each visible node to a text file.  The set of
# visible nodes is chosen by the user at runtime via the
# "Visible Nodes" property.

import k3d

k3d.check_node_environment(locals(), "RenderEngineScript")

output = open(str(OutputImage), "w")

for node in VisibleNodes:
	output.write(k3d.dynamic_cast(node, "iproperty_collection").name + "\n")

output.close()

