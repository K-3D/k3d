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

	connect_vertices = doc.new_node("ConnectVertices")
	selection = k3d.mesh_selection.deselect_all()
	selection.points = [(0, 1, 1), (1, 2, 0), (2, 3, 1), (3, 1000, 0)]
	connect_vertices.mesh_selection = selection

	diff_input = diff.create_property("k3d::mesh*", "input_" + str(i), "Input " + str(i), "Input mesh " + str(i))

	doc.set_dependency(connect_vertices.get_property("input_mesh"), cube.get_property("output_mesh"));
	doc.set_dependency(diff_input, connect_vertices.get_property("output_mesh"));

if not diff.equal:
	raise Exception("Input meshes differ")

