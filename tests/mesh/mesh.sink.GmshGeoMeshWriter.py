#python

import k3d
import testing

target_file = "mesh.sink.GmshGeoMeshWriter.geo"

setup = testing.setup_mesh_test(["PolyCube", "GmshGeoMeshWriter"])
setup.sink.file = k3d.filesystem.generic_path(testing.binary_path() + "/meshes/" + target_file)

