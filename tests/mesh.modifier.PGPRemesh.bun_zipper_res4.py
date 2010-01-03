#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PLYMeshReader", "PGPRemesh")
setup.source.file = testing.source_path() + "/meshes/bun_zipper_res4.ply"
#setup.modifier.use_smooth = True
#setup.modifier.smooth_4 = False
#setup.modifier.steps = 5
#setup.modifier.h = 1500
#setup.modifier.omega = 10
#setup.modifier.div = 2

testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.PGPRemesh.bun_zipper_res4", 1)

