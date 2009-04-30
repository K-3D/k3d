#python

import testing
import k3d

factories = k3d.plugin.factory.lookup()

failing_document_importers = ""

doc = None

for factory in factories :
	if factory.is_application_plugin() and ("DocumentImporter" in factory.name()):
		try:
			document_importer = k3d.plugin.create(factory.name())
			path = k3d.filesystem.generic_path(testing.source_path() + "/meshes/" + "mesh.source.MeshReaders.bogus_input")
			doc = k3d.new_document()
			document_importer.read_file(doc,path)
			k3d.close_document(doc)
			doc = None
		except:
			failing_document_importers += factory.name() + " "
			if doc != None:
				k3d.close_document(doc)

if failing_document_importers != "":
	raise Exception(failing_document_importers + "failed to load a bogus input")
