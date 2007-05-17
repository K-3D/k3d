#python

import k3d

doc = k3d.new_document()

memory_pools = k3d.create_plugin("MemoryPools").dynamic_cast("icommand_node")
memory_pools.execute_command("shuffle", "4000 points")
memory_pools.execute_command("shuffle", "4000 split_edges")
memory_pools.execute_command("shuffle", "4000 faces")

diff = doc.new_node("MeshDiff")

for i in range(100):
	cube = doc.new_node("PolyCube")

	extrude_faces = doc.new_node("ExtrudeFaces")
	extrude_faces.region = False
	selection = k3d.deselect_all()
	selection.faces = [(0, 3, 1), (3, 1000, 0)]
	extrude_faces.mesh_selection = selection

	diff_input = diff.add_user_property("k3d::mesh*", "input_" + str(i), "Input " + str(i), "Input mesh " + str(i))

	doc.set_dependency(extrude_faces.get_property("input_mesh"), cube.get_property("output_mesh"));
	doc.set_dependency(diff_input, extrude_faces.get_property("output_mesh"));

if not diff.equal:
	raise Exception("Input meshes differ")

