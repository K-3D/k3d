#python

import testing

setup = testing.setup_mesh_source_test("PolyCylinder")

setup.source.top_segments = 2
setup.source.bottom_segments = 2

testing.mesh_comparison_to_reference(setup.document, setup.source.get_property("output_mesh"), "mesh.source.PolyCylinder.caps", 1, testing.platform_specific)

