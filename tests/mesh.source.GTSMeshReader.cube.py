#python

import testing

setup = testing.setup_mesh_reader_test("GTSMeshReader", "mesh.source.GTSMeshReader.cube.gts")
testing.mesh_comparison_to_reference(setup.document, setup.reader.get_property("output_mesh"), "mesh.source.GTSMeshReader.cube", 1)

