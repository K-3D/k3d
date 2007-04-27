#python

import k3d
import sys

for node in Document.nodes():
	if node.selection_weight:
		print "Selected:", node.name

sys.stdout.flush()

k3d.ui.message("Output sent to console")

