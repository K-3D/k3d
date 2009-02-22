#python

import testing

setup = testing.setup_mesh_source_test("PolyDisk")
testing.assert_contains_solid_polyhedron(setup.output_mesh)

