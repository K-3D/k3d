#python

import k3d

doc = Document
doc.start_change_set()
try:
	material = doc.new_node("RenderManMaterial")
	material.name = "Patch Material"
	material.color = k3d.color(1, 1, 1)

	frozen_mesh = doc.new_node("FrozenMesh")
	frozen_mesh.name = "Bilinear Patch"

	mesh = k3d.dynamic_cast(frozen_mesh, "imesh_storage").reset_mesh()

	positions = [(-5, 0, 5), (5, 0, 5), (0, -5, -5), (0, 5, -5)]

	points = mesh.create_points()
	point_selection = mesh.create_point_selection()

	for position in positions:
		points.append(k3d.point3(position[0], position[1], position[2]))
		point_selection.append(0.0)

	bilinear_patches = mesh.create_bilinear_patches()

	patch_selection = bilinear_patches.create_patch_selection()
	patch_selection.assign([0])

	patch_materials = bilinear_patches.create_patch_materials()
	patch_materials.assign([None])

	patch_points = bilinear_patches.create_patch_points()
	patch_points.assign([0, 1, 2, 3])

	Cs = bilinear_patches.writable_varying_data().create_array("Cs", "k3d::color")
	Cs.assign([k3d.color(1, 0, 0), k3d.color(0, 1, 0), k3d.color(0, 0, 1), k3d.color(1, 1, 1)])

	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.name = "Bilinear Patch Instance"
	mesh_instance.gl_painter = doc.get_node("GL Default Painter")
	mesh_instance.ri_painter = doc.get_node("RenderMan Default Painter")
	doc.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	doc.finish_change_set("Create Bilinear Patch")

except:
	doc.cancel_change_set()
	raise

