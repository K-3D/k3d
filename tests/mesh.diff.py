#python

import k3d

doc = k3d.new_document()

source_a = doc.new_node("PolyGrid")
source_b = doc.new_node("PolyGrid")

diff = doc.new_node("MeshDiff")
diff.add_user_property("k3d::dev::mesh*", "input_a", "InputA", "First input mesh")
diff.add_user_property("k3d::dev::mesh*", "input_b", "InputB", "Second input mesh")

doc.set_dependency(diff.get_property("input_a"), source_a.get_property("output_mesh"));
doc.set_dependency(diff.get_property("input_b"), source_b.get_property("output_mesh"));

if not diff.equal:
	print "source a " + repr(source_a.output_mesh)
	print "source b " + repr(source_b.output_mesh)
	raise Exception("Input meshes should test equal")

source_a.columns += 1

if diff.equal:
	print "source a " + repr(source_a.output_mesh)
	print "source b " + repr(source_b.output_mesh)
	raise Exception("Input meshes should test not equal")

