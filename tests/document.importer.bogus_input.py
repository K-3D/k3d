#python

import testing
import k3d

factories = k3d.plugin.factory.lookup()

for factory in factories :
	if factory.is_application_plugin() and ("DocumentImporter" in factory.name()):

		print "\n\nTesting " + factory.name() + " with a file containing all zeroes ..."
		path = k3d.filesystem.generic_path(testing.source_path() + "/meshes/" + "zero_bytes")
		document = k3d.new_document()
		document_importer = k3d.plugin.create(factory.name())
		document_importer.read_file(document, path)
		k3d.close_document(document)

		print "\n\nTesting " + factory.name() + " with a file containing random data ..."
		path = k3d.filesystem.generic_path(testing.source_path() + "/meshes/" + "random_bytes")
		document = k3d.new_document()
		document_importer = k3d.plugin.create(factory.name())
		document_importer.read_file(document, path)
		k3d.close_document(document)

