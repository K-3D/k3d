#python

import k3d
import testing

setup = testing.setup_mesh_writer_test(["PolyCube", "AddIndexAttributes", "K3DMeshWriter"], "K3DMeshReader", "mesh.sink.K3DMeshWriter.k3d")

testing.require_valid_mesh(setup.document, setup.reader.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.reader.get_property("output_mesh"), "mesh.sink.K3DMeshWriter", 1)

