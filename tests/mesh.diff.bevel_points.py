#python

import k3d

doc = k3d.application.new_document()

memory_pools = k3d.create_plugin("MemoryPools").dynamic_cast("icommand_node")
memory_pools.execute_command("shuffle", "4000 points")
memory_pools.execute_command("shuffle", "4000 split_edges")
memory_pools.execute_command("shuffle", "4000 faces")

diff = doc.new_node("MeshDiff")

for i in range(100):
	cube = doc.new_node("PolyCube")

	bevel_points = doc.new_node("BevelPoints")
	selection = k3d.deselect_all()
	selection.points = [(0, 4, 1), (4, 1000, 0)]
	bevel_points.mesh_selection = selection;

	diff_input = diff.add_user_property("k3d::dev::mesh*", "input_" + str(i), "Input " + str(i), "Input mesh " + str(i))

	doc.set_dependency(bevel_points.get_property("input_mesh"), cube.get_property("output_mesh"));
	doc.set_dependency(diff_input, bevel_points.get_property("output_mesh"));

if not diff.equal:
	raise Exception("Input meshes differ")

