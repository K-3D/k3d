#python

import testing
import k3d

factories = k3d.plugin.factory.lookup()

failing_factories = ""

for factory in factories:
  if "MeshReader" in factory.categories():
    try:
      setup = testing.setup_mesh_reader_test(factory.name(), "mesh.source.MeshReaders.bogus_input")
      mesh = setup.reader.output_mesh
    except:
      failing_factories += factory.name() + " "

if len(failing_factories) > 0:
  raise Exception(failing_factories + "failed to load a bogus input")