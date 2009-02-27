#python

import k3d
import testing

doc = k3d.new_document()

# We will be writing a temporary file ...
file = k3d.generic_path(testing.binary_path() + "/mesh.sink.PLYMeshWriter.ply")

# Create a simple polyhedron source ...
source = doc.new_node("PolyCube")

# Write the geometry to a temporary file ...
writer = doc.new_node("PLYMeshWriter")
writer.file = file
doc.set_dependency(writer.get_property("input_mesh"), source.get_property("output_mesh"))

# Read the geometry back in ...
reader = doc.new_node("PLYMeshReader")
reader.file = file
reader.center = False
reader.scale_to_size = False

# Compare the newly-loaded geometry to a reference (we don't use the original PolyCube mesh source
# because PLYMeshWriter triangulates its input).
testing.mesh_comparison_to_reference(doc, reader.get_property("output_mesh"), "mesh.sink.PLYMeshWriter", 1)

