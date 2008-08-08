#python

import testing

setup = testing.setup_mesh_reader_test("3DSMeshReader", "mesh.source.3DSMeshReader.test.3ds")
testing.mesh_comparison_to_reference(setup.document, setup.reader.get_property("output_mesh"), "mesh.source.3DSMeshReader.test", 1)

