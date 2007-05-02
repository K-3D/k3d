#python

import k3d

if locals().has_key("Node"):
	k3d.ui().message("This script is executing within a node context")
elif locals().has_key("Document"):
	k3d.ui().message("This script is executing within a document context")
else:
	k3d.ui().message("This script is executing within the global context")

