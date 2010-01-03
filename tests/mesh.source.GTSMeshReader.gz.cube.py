#python

import testing

setup = testing.setup_mesh_reader_test("GTSMeshReader", "mesh.source.GTSMeshReader.cube.gts.gz")

testing.require_valid_primitives(setup.document, setup.source.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.GTSMeshReader.gz.cube", 1)

