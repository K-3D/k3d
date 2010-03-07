#python

import k3d

doc = k3d.new_document()

axes = k3d.plugin.create("Axes", doc)
doc.set_dependency(axes.get_property("xyplane"), axes.get_property("axes"));

if doc.get_dependency(axes.get_property("xyplane")).name() != "axes":
	raise "xyplane dependency should be axes"

if doc.get_dependency(axes.get_property("yzplane")):
	raise "yzplane dependency should be null"

