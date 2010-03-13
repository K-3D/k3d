#python

import k3d

doc = k3d.new_document()

axes = k3d.plugin.create("Axes", doc)
axes.axes = True
axes.xyplane = False

# Confirm that setting a property dependency works ...
k3d.property.connect(doc, axes.get_property("axes"), axes.get_property("xyplane"));

if axes.get_property("xyplane").internal_value() != False:
	raise "incorrect internal value"

if axes.get_property("xyplane").pipeline_value() != True:
	raise "incorrect pipeline value"

# Confirm that removing a property dependency works ...
k3d.property.disconnect(doc, axes.get_property("xyplane"))

if axes.get_property("xyplane").pipeline_value() != False:
	raise "incorrect pipeline value"
