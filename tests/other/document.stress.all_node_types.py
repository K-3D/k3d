#python

import k3d
import sys

doc = k3d.new_document();

for factory in sorted(k3d.plugin.factory.lookup(), lambda x, y: x.name() < y.name()):
	if factory.is_document_plugin():
		k3d.log.info("Creating " + factory.name())
		node = doc.new_node(factory)

