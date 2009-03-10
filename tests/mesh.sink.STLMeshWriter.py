#python

import k3d
import testing

doc = k3d.new_document()

# We will be writing a temporary file ...
file = k3d.filesystem.generic_path(testing.binary_path() + "/mesh.sink.STLMeshWriter.stl")

# Create a simple polyhedron source ...
cube = doc.new_node("PolyCube")
#triangulate it
source = doc.new_node("TriangulateFaces")
source.mesh_selection = k3d.mesh_selection.select_all()
doc.set_dependency(source.get_property("input_mesh"), cube.get_property("output_mesh"))

# Write the geometry to a temporary file ...
writer = doc.new_node("STLMeshWriter")
writer.file = file
doc.set_dependency(writer.get_property("input_mesh"), source.get_property("output_mesh"))

# Read the geometry back in ...
reader = doc.new_node("STLMeshReader")
reader.file = file
reader.center = False
reader.scale_to_size = False

#read in the reference date. This differs from the source data, since the STL file format is incapable of preserving vertex order
reference = doc.new_node("K3DMeshReader")
reference.file = k3d.filesystem.generic_path(testing.source_path() + "/meshes/mesh.source.STLMeshReader.reference.1.k3d")
reference.center = False
reference.scale_to_size = False

# Compare the reference to the imported data ...
diff = doc.new_node("MeshDiff")
diff.create_property("k3d::mesh*", "input_a", "InputA", "First input mesh")
diff.create_property("k3d::mesh*", "input_b", "InputB", "Second input mesh")

doc.set_dependency(diff.get_property("input_a"), reference.get_property("output_mesh"))
doc.set_dependency(diff.get_property("input_b"), reader.get_property("output_mesh"))

if not diff.get_property("input_a").pipeline_value() or not diff.get_property("input_b").pipeline_value():
	raise Exception("missing mesh comparison input")

if not diff.equal:
	print "source " + repr(reference.output_mesh)
	print "reader " + repr(reader.output_mesh)
	raise Exception("Exported mesh differs")

