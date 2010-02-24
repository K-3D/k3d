#python

import k3d
import testing

setup = testing.setup_mesh_writer_test(["PolyCube", "OBJMeshWriter"], "OBJMeshReader", "mesh.sink.OBJMeshWriter.obj")

testing.require_valid_mesh(setup.document, setup.reader.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.reader.get_property("output_mesh"), "mesh.sink.OBJMeshWriter", 1)
