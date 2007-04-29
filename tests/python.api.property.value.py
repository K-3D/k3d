#python

import k3d

doc = k3d.application.new_document()

axes = doc.new_node("Axes")
axes.axes = True
axes.xyplane = False
doc.set_dependency(axes.get_property("xyplane"), axes.get_property("axes"));

if axes.get_property("xyplane").internal_value() != False:
	raise "incorrect internal value"

if axes.get_property("xyplane").value() != True:
	raise "incorrect value"

