#python

import k3d

doc = k3d.new_document()

source_a = k3d.plugin.create("PolyGrid", doc)
source_b = k3d.plugin.create("PolyGrid", doc)

diff = k3d.plugin.create("MeshDiff", doc)
k3d.property.create(diff, "k3d::mesh*", "input_a", "InputA", "First input mesh")
k3d.property.create(diff, "k3d::mesh*", "input_b", "InputB", "Second input mesh")

k3d.property.connect(doc, source_a.get_property("output_mesh"), diff.get_property("input_a"));
k3d.property.connect(doc, source_b.get_property("output_mesh"), diff.get_property("input_b"));

if not diff.equal:
	print "source a " + repr(source_a.output_mesh)
	print "source b " + repr(source_b.output_mesh)
	raise Exception("Input meshes should test equal")

source_a.columns += 1

if diff.equal:
	print "source a " + repr(source_a.output_mesh)
	print "source b " + repr(source_b.output_mesh)
	raise Exception("Input meshes should test not equal")

