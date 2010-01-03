#python

import testing

setup = testing.setup_mesh_source_test("LSystemParser")
setup.source.growth = 2
testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.LSystemParser", 1, testing.platform_specific)

