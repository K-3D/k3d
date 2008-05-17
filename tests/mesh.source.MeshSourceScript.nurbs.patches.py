#python

import k3d
import testing

script_path = k3d.share_path() / k3d.generic_path("scripts/MeshSourceScript/nurbs_patches.py")
script_file = file(str(script_path), "r")

setup = testing.setup_mesh_source_test("MeshSourceScript")
setup.source.script = script_file.read()

testing.mesh_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.MeshSourceScript.nurbs.patches", 2)

