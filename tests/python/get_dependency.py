#python

import k3d

doc = k3d.new_document()

axes = k3d.plugin.create("Axes", doc)
k3d.property.connect(doc, axes.get_property("axes"), axes.get_property("xyplane"));

if doc.get_dependency(axes.get_property("xyplane")).name() != "axes":
	raise "xyplane dependency should be axes"

if doc.get_dependency(axes.get_property("yzplane")):
	raise "yzplane dependency should be null"

