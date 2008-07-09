#python

import testing

setup = testing.setup_mesh_source_test("SphereEversion")
setup.source.time = 0.25
testing.mesh_comparison_to_reference(setup.document, setup.source.get_property("output_mesh"), "mesh.source.SphereEversion", 48)

