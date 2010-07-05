#python

import k3d
import testing

setup = testing.setup_mesh_writer_test(["PolyCube", "TriangulateFaces", "STLMeshWriter"], "STLMeshReader", "mesh.sink.STLMeshWriter.magics.stl")
setup.modifier.mesh_selection = k3d.geometry.selection.create(1)
setup.writer.file_type = "magics"

testing.require_valid_mesh(setup.document, setup.reader.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.reader.get_property("output_mesh"), "mesh.sink.STLMeshWriter.magics", 1)
