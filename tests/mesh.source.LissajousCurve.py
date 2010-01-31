#python

import testing

setup = testing.setup_mesh_source_test("LissajousCurve")

testing.require_valid_mesh(setup.document, setup.source.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.source.get_property("output_mesh"), "mesh.source.LissajousCurve", 1, testing.platform_specific)

