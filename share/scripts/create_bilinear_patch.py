#python

import k3d

context.document.start_change_set()
try:
	# Create a FrozenMesh node to act as a mesh source ...
	frozen_mesh = context.document.new_node("FrozenMesh")
	frozen_mesh.name = "Bilinear Patch"

	# Create a mesh ...
	mesh = frozen_mesh.create_mesh()

	# Add geometric points to the mesh ...
	points = mesh.create_points()
	point_selection = mesh.create_point_selection()

	positions = [(-5, 0, 5), (5, 0, 5), (0, -5, -5), (0, 5, -5)]
	for position in positions:
		points.append(k3d.point3(position[0], position[1], position[2]))
		point_selection.append(0.0)

	# Create a bilinear_patch primitive ...
	patches = k3d.bilinear_patch.create(mesh)

	# Create a custom attribute array to store color values at each parametric corner of each patch ...
	Cs = patches.parameter_attributes().create("Cs", "k3d::color")

	# Add a single patch to the primitive ...
	patches.patch_selections().append(0)
	patches.patch_materials().append(None)
	for i in range(4):
		patches.patch_points().append(len(patches.patch_points()))

	# Add some colors to the patch corners ...
	Cs.append(k3d.color(1, 0, 0))
	Cs.append(k3d.color(0, 1, 0))
	Cs.append(k3d.color(0, 0, 1))
	Cs.append(k3d.color(1, 1, 1))

	# Connect the FrozenMesh to a MeshInstance to place it in the scene ...
	mesh_instance = context.document.new_node("MeshInstance")
	mesh_instance.name = "Bilinear Patch Instance"
	mesh_instance.gl_painter = context.document.get_node("GL Default Painter")
	mesh_instance.ri_painter = context.document.get_node("RenderMan Default Painter")

	context.document.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	# Make the MeshInstance visible to render engines ...
	k3d.node.show(context.document, mesh_instance)

	context.document.finish_change_set("Create Bilinear Patch")

except:
	context.document.cancel_change_set()
	raise

