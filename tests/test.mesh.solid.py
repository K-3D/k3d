#python

import testing

setup = testing.setup_mesh_source_test("PolyCube")
testing.assert_contains_solid_polyhedron(setup.output_mesh)

