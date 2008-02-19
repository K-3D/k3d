#python

import k3d

doc = k3d.new_document()

# Create a simple polyhedron source ...
source = doc.new_node("PolyCube")

# Select some geometry ...
select_face = doc.new_node("SelectFaceByNumber")
select_face.index = 1;
doc.set_dependency(select_face.get_property("input_mesh"), source.get_property("output_mesh"))

select_edge = doc.new_node("SelectEdgeByNumber")
select_edge.index = 2;
doc.set_dependency(select_edge.get_property("input_mesh"), select_face.get_property("output_mesh"))

select_point = doc.new_node("SelectPointByNumber")
select_point.index = 3;
doc.set_dependency(select_point.get_property("input_mesh"), select_edge.get_property("output_mesh"))

# Run the geometry through a conversion from mesh to legacy-mesh and back ...
convert = doc.new_node("LegacyMeshConversion")
doc.set_dependency(convert.get_property("input_mesh"), select_point.get_property("output_mesh"))

# Compare the original to the conversion ...
diff = doc.new_node("MeshDiff")
diff.create_property("k3d::mesh*", "input_a", "InputA", "First input mesh")
diff.create_property("k3d::mesh*", "input_b", "InputB", "Second input mesh")

doc.set_dependency(diff.get_property("input_a"), select_point.get_property("output_mesh"))
doc.set_dependency(diff.get_property("input_b"), convert.get_property("output_mesh"))

if not diff.equal:
	print "source " + repr(select_point.output_mesh)
	print "converted " + repr(convert.output_mesh)
	raise Exception("Converted mesh differs")

