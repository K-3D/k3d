#python

import k3d

doc = k3d.new_document()

axes = doc.new_node("Axes")
axes.selection_weight = 0.5
if axes.selection_weight != 0.5:
	raise "incorrect node selection_weight"

