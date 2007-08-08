#python

import testing

setup = testing.setup_mesh_source_test("PolyDisk")
testing.assert_solid_mesh(setup.output_mesh)

