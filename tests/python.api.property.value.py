#python

import k3d

doc = k3d.new_document()

axes = doc.new_node("Axes")
axes.axes = True
axes.xyplane = False

# Confirm that setting a property dependency works ...
doc.set_dependency(axes.get_property("xyplane"), axes.get_property("axes"));

if axes.get_property("xyplane").internal_value() != False:
	raise "incorrect internal value"

if axes.get_property("xyplane").pipeline_value() != True:
	raise "incorrect pipeline value"

# Confirm that removing a property dependency works ...
doc.set_dependency(axes.get_property("xyplane"), None)

if axes.get_property("xyplane").pipeline_value() != False:
	raise "incorrect pipeline value"
