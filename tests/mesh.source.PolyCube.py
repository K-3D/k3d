#python

import testing

setup = testing.setup_mesh_source_test("PolyCube")

testing.assert_contains_solid_polyhedron(setup.output_mesh)
testing.mesh_comparison_to_reference(setup.document, setup.source.get_property("output_mesh"), "mesh.source.PolyCube", 1)

