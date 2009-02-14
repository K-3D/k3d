#python

import k3d

doc = k3d.new_document()

memory_pools = k3d.plugin.create("MemoryPools")
memory_pools.execute_command("shuffle", "4000 points")
memory_pools.execute_command("shuffle", "4000 split_edges")
memory_pools.execute_command("shuffle", "4000 faces")

diff = doc.new_node("MeshDiff")

for i in range(100):
	cube = doc.new_node("PolyCube")

	subdivide_edges = doc.new_node("SubdivideEdges")
	selection = k3d.mesh_selection.deselect_all()
	selection.edges = [(0, 4, 1), (4, 1000, 0)]
	subdivide_edges.mesh_selection = selection

	diff_input = diff.create_property("k3d::mesh*", "input_" + str(i), "Input " + str(i), "Input mesh " + str(i))

	doc.set_dependency(subdivide_edges.get_property("input_mesh"), cube.get_property("output_mesh"));
	doc.set_dependency(diff_input, subdivide_edges.get_property("output_mesh"));

if not diff.equal:
	raise Exception("Input meshes differ")

