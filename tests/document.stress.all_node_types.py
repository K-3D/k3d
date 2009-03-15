#python

import k3d
import sys

doc = k3d.new_document();

factories = k3d.plugin.factory.lookup()
for factory in factories:
	if factory.is_document_plugin():
		sys.stderr.write(factory.name() + "\n")
		sys.stderr.flush()

		node = doc.new_node(factory)

