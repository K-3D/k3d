#python

import k3d

doc = Document
doc.start_change_set()
try:
	frozen_mesh = doc.new_node("FrozenMesh")
	frozen_mesh.name = "Blobby"

	mesh = frozen_mesh.new_mesh()
	positions = ((-.5, .5, 0), (.5, .5, 0), (.5, -.5, 0), (-.5, -.5, 0))
	for position in positions:
		mesh.new_point(position)

	blobby = mesh.new_blobby()

	ellipsoids = []
	for point in mesh.points:
		ellipsoids.append(blobby.new_ellipsoid(point))

	blobby.root = blobby.new_add(ellipsoids)

	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.name = "Blobby Instance"

	scale = doc.new_node("Scale")
	scale.name = "Blobby Scale"
	scale.x = 5.0
	scale.y = 5.0
	scale.z = 5.0

#	mesh_instance.scale = (5, 5, 5)
	doc.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))
	doc.set_dependency(mesh_instance.get_property("input_matrix"), scale.get_property("output_matrix"))

	doc.finish_change_set("Create Test Blobby")

	k3d.ui.message("Note: not all RenderMan engines can render blobby geometry!")

except:
	doc.cancel_change_set()
	raise

