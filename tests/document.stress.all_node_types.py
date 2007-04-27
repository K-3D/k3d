#python

import k3d
doc = k3d.application.new_document();

factories = k3d.plugins();
for factory in factories:
	if factory.is_document_plugin:
		node = doc.new_node(factory)

