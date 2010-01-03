#python

import testing

setup = testing.setup_mesh_reader_test("3DSMeshReader", "mesh.source.3DSMeshReader.test.3ds")

testing.require_valid_primitives(setup.document, setup.source.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.3DSMeshReader.test", 1)

