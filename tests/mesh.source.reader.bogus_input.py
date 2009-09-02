#python

import testing
import k3d

factories = k3d.plugin.factory.lookup()

for factory in factories:
	if "MeshReader" in factory.categories():

		print "\n\nTesting " + factory.name() + " with a file containing all zeroes ..."
		setup = testing.setup_mesh_reader_test(factory.name(), "zero_bytes")
		mesh = setup.reader.output_mesh

		print "\n\nTesting " + factory.name() + " with a file containing random data ..."
		setup = testing.setup_mesh_reader_test(factory.name(), "random_bytes")
		mesh = setup.reader.output_mesh

