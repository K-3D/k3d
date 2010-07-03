#python

import k3d

if context.has_key("node"):
	k3d.ui().message("This script is executing within a node context")
elif context.has_key("document"):
	k3d.ui().message("This script is executing within a document context")
else:
	k3d.ui().message("This script is executing within the global context")

