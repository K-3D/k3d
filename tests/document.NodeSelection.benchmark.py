#python

import k3d

doc = k3d.new_document()
node_selection = doc.new_node("NodeSelection")
node_selection.name = "NodeSelection"
node_selection.selection_weight = 1.0
for i in range(0, 1000):
	node = doc.new_node("FrozenMesh")
	node.name = "Node " + str(i)
	node_selection.set_selection_node = node
	
profiler = doc.new_node("PipelineProfiler")
node = doc.new_node("FrozenMesh")
node.name = "Last FrozenMesh"

node_selection.set_selection_node = node
node_selection.get_selection_node = node

records = profiler.records.values()[0]

print '<DartMeasurement name="Set selection" type="text/string">' + str(records["Set Node Selection"]) + '</DartMeasurement>'
print '<DartMeasurement name="Get selection" type="text/string">' + str(records["Get Node Selection"]) + '</DartMeasurement>'
	

