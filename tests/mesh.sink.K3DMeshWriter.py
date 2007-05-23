#python

import k3d

doc = k3d.new_document()

# We will be writing a temporary file ...
file = "@k3d-tests_BINARY_DIR@/mesh.sink.K3DMeshWriter.k3d"

# Create a simple polyhedron source ...
source = doc.new_node("PolyCube")

# Write the geometry to a temporary file ...
writer = doc.new_node("K3DMeshWriter")
writer.file = file
doc.set_dependency(writer.get_property("input_mesh"), source.get_property("output_mesh"))

# Read the geometry back in ...
reader = doc.new_node("K3DMeshReader")
reader.file = file

# Compare the original to the imported data ...
diff = doc.new_node("MeshDiff")
diff.add_user_property("k3d::mesh*", "input_a", "InputA", "First input mesh")
diff.add_user_property("k3d::mesh*", "input_b", "InputB", "Second input mesh")

doc.set_dependency(diff.get_property("input_a"), source.get_property("output_mesh"))
doc.set_dependency(diff.get_property("input_b"), reader.get_property("output_mesh"))

if not diff.equal:
	print "source " + repr(source.output_mesh)
	print "reader " + repr(reader.output_mesh)
	raise Exception("Exported mesh differs")

