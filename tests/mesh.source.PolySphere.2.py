#python

import testing

setup = testing.setup_mesh_source_test("PolySphere")

setup.source.type = "quad_only_sphere"

testing.mesh_comparison_to_reference(setup.document, setup.source.get_property("output_mesh"), "mesh.source.PolySphere.2", 1, testing.platform_specific)

