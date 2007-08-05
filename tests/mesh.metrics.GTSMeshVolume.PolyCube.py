#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "GTSMeshVolume")
setup.source.width = 2
setup.source.height = 3
setup.source.depth = 4

testing.mesh_volume_comparison(setup.modifier.volume, 24.0)

