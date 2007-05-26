#python

import k3d

doc = Document
doc.start_change_set()
try:
	material = doc.new_node("RenderManMaterial")
	material.name = "Patch Material"
	material.color = k3d.color(1, 1, 1)

	frozen_mesh = doc.new_node("FrozenMesh")
	frozen_mesh.name = "Bicubic Patch"

	mesh = k3d.dynamic_cast(frozen_mesh, "imesh_storage").new_mesh()

	positions = [
		(-5, -5, 0), (-2, -5, 2), (2, -5, -2), (5, -5, 0),
		(-5, -2, 2), (-2, -2, 5), (2, -2, -5), (5, -2, -2),
		(-5, 2, 2), (-2, 2, 5), (2, 2, -5), (5, 2, -2),
		(-5, 5, 0), (-2, 5, 2), (2, 5, -2), (5, 5, 0)
		]

	points = mesh.create_points()
	point_selection = mesh.create_point_selection()

	for position in positions:
		points.append(k3d.point3(position[0], position[2], -position[1]))

	bicubic_patches = mesh.create_bicubic_patches()

	patch_selection = bicubic_patches.create_patch_selection()
	patch_selection.append(0)

	patch_materials = bicubic_patches.create_patch_materials()
	patch_materials.append(k3d.dynamic_cast(material, "imaterial"))

	patch_points = bicubic_patches.create_patch_points()
	for i in range(16):
		patch_points.append(i)

	Cs = bicubic_patches.writable_varying_data().create_array("Cs", "k3d::color")
	Cs.assign([k3d.color(1, 0, 0), k3d.color(0, 1, 0), k3d.color(0, 0, 1), k3d.color(1, 1, 1)])

	mesh_instance = doc.new_node("MeshInstance")
	mesh_instance.name = "Bicubic Patch Instance"
	mesh_instance.gl_painter = doc.get_node("GL Default Painter")
	mesh_instance.ri_painter = doc.get_node("RenderMan Default Painter")

	doc.set_dependency(mesh_instance.get_property("input_mesh"), frozen_mesh.get_property("output_mesh"))

	doc.finish_change_set("Create Bicubic Patch")

except:
	doc.cancel_change_set()
	raise

