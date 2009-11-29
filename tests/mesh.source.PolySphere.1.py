#python

import testing

setup = testing.setup_mesh_source_test("PolySphere")

setup.source.type = "sphere"

testing.mesh_comparison_to_reference(setup.document, setup.source.get_property("output_mesh"), "mesh.source.PolySphere.1", 1, testing.platform_specific)
