#python

# Creates one of every type of document plugin, and reads the value from every property,
# which should cause lots of internals to be executed

import k3d
import sys

result = 0

doc = k3d.new_document()

unsupported_types = [ "k3d::legacy::mesh*", "std::vector<k3d::point3>", "k3d::typed_array<unsigned int>", "std::vector<unsigned int>", "" ]

factories = k3d.plugins()
for factory in factories:
	if factory.is_document_plugin():
		node = doc.new_node(factory)
		for property in node.properties():
			try:
				if property.type() in unsupported_types:
					continue

				value = property.internal_value()
			except:
				print "Error reading value from " + factory.name() + "." + property.name() + " [" + property.type() + "]"
				result = 1

sys.exit(result)

