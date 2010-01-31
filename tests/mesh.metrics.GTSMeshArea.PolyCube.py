#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "GTSMeshArea")
setup.source.width = 1
setup.source.height = 1
setup.source.depth = 1

testing.require_mesh_area(setup.modifier.area, 6.0)

