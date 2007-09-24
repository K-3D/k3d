#python

import k3d
import testing

document = k3d.new_document()

reader = document.new_node("PLYMeshReader")
reader.file = "@CMAKE_CURRENT_SOURCE_DIR@/meshes/bun_zipper_res4.ply"

modifier = document.new_node("PGPRemesh")
#modifier.use_smooth = True
#modifier.smooth_4 = False
#modifier.steps = 5
#modifier.h = 1500
#modifier.omega = 10
#modifier.div = 2
document.set_dependency(modifier.get_property("input_mesh"), reader.get_property("output_mesh"))

testing.mesh_comparison(document, modifier.get_property("output_mesh"), "mesh.modifier.PGPRemesh.bun_zipper_res4", 1)

