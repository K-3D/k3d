#python

import k3d

doc = k3d.new_document()

node1 = k3d.plugin.create("TwistPoints", doc)
node2 = k3d.plugin.create("TwistPoints", doc)

node1.axis = "x"
node2.axis = "y"

doc.set_dependency(node2.get_property("axis"), node1.get_property("axis"));

if node1.get_property("axis").internal_value() != "x":
	raise "incorrect internal value"

if node2.get_property("axis").internal_value() != "y":
	raise "incorrect internal value"

if node1.get_property("axis").pipeline_value() != "x":
	raise "incorrect pipeline value"

if node2.get_property("axis").pipeline_value() != "x":
	raise "incorrect pipeline value"

