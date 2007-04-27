#python

import k3d

doc = k3d.application.new_document()

axes = doc.new_node("Axes")
doc.set_dependency(axes.get_property("xyplane"), axes.get_property("axes"));

if doc.get_dependency(axes.get_property("xyplane")).name != "axes":
	raise "xyplane dependency should be axes"

if doc.get_dependency(axes.get_property("yzplane")):
	raise "yzplane dependency should be null"

