#python

import k3d

for node in Document.nodes():
	if node.selection_weight:
		print "Selected:", node.name

import sys
sys.stdout.flush()

k3d.ui().message("Output sent to console")

