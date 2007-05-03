#python

import k3d

doc = k3d.new_document()

# Create some bilinear patches ...
poly_source = doc.new_node("PolyCube")

source = doc.new_node("MakeBilinearPatches")
doc.set_dependency(source.get_property("input_mesh"), poly_source.get_property("output_mesh"))

# Select some geometry ...
selection = doc.new_node("SelectBicubicPatchByNumber")
selection.index = 0;
doc.set_dependency(selection.get_property("input_mesh"), source.get_property("output_mesh"))

# Run the geometry through a conversion from mesh to legacy-mesh and back ...
convert = doc.new_node("LegacyMeshConversion")
doc.set_dependency(convert.get_property("input_mesh"), selection.get_property("output_mesh"))

# Compare the original to the conversion ...
diff = doc.new_node("MeshDiff")
diff.add_user_property("k3d::dev::mesh*", "input_a", "InputA", "First input mesh")
diff.add_user_property("k3d::dev::mesh*", "input_b", "InputB", "Second input mesh")

doc.set_dependency(diff.get_property("input_a"), selection.get_property("output_mesh"))
doc.set_dependency(diff.get_property("input_b"), convert.get_property("output_mesh"))

if not diff.equal:
	print "source " + repr(selection.output_mesh)
	print "converted " + repr(convert.output_mesh)
	raise Exception("Converted mesh differs")

