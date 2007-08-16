#python

import k3d
k3d.check_node_environment(locals(), "MeshSourceScript")

positions = [(-5, 0, 5), (5, 0, 5), (0, -5, -5), (0, 5, -5)]

points = Output.create_points()
point_selection = Output.create_point_selection()

for position in positions:
	points.append(k3d.point3(position[0], position[1], position[2]))
	point_selection.append(0.0)

bilinear_patches = Output.create_bilinear_patches()

patch_selection = bilinear_patches.create_patch_selection()
patch_selection.assign([0])

patch_materials = bilinear_patches.create_patch_materials()
patch_materials.assign([None])

patch_points = bilinear_patches.create_patch_points()
patch_points.assign([0, 1, 2, 3])

Cs = bilinear_patches.writable_varying_data().create_array("Cs", "k3d::color")
Cs.assign([k3d.color(1, 0, 0), k3d.color(0, 1, 0), k3d.color(0, 0, 1), k3d.color(1, 1, 1)])

