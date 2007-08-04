#python

import testing

setup = testing.setup_mesh_source_test("PolyDisk")
testing.assert_solid_mesh(setup.source.get_property("output_mesh"))

