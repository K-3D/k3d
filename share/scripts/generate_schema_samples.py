#python

import k3d
import sys

print """<?xml version="1.0"?>
<!--

Sample K-3D XML document dynamically generated using the generate_schema_samples.py script - do not edit by hand!

This document is intended as an informal sample of the types of data that could be contained within a K-3D XML
document.  For a formal description of the K-3D XML schema, see k3d/docs/xml_schema/schema.xsd.  This document will
be updated from time-to-time as the list of available plugin types grows.  Keep in mind that the set of properties
for each plugin type may change as new features are added, and that some types (e.g. the RenderManXXXXXShader types)
will have varying sets of properties at runtime.  You do not have to specify every property for a node - sensible
defaults will be used at runtime.  The default values for properties are the ones shown here.

The node class attribute is a 128 bit universally unique number that unambiguously identifies plugin type.

Note that the node name attribute is the human-readable node name that is visible within the user interface -
node names are not guaranteed unique within a document!  Use the object id attribute if you need a unique identifier.

All data that is expressed using real-world units of measure use SI units.  All angles are measured in radians.

-->

<k3dml version="0.5.0.0" generator="generate_schema_samples.py">
\t<document>
\t\t<nodes>"""

doc = k3d.application.new_document()
unique_id = 1

plugins = {}
for plugin in k3d.plugins():
	if plugin.is_document_plugin():
		plugins[plugin.name()] = plugin

sorted_plugins = plugins.keys()
sorted_plugins.sort()

for plugin_name in sorted_plugins:
	plugin = plugins[plugin_name]

	class_id = plugin.class_id()
	node = doc.new_node(plugin.name())
	node.name = plugin.name()

	print """\t\t\t<!-- %s -->""" % plugin.short_description()
	print """\t\t\t<node name=\"%s\" id=\"%u\" class=\"%.08x %.08x %.08x %.08x\">""" % (plugin.name(), unique_id, class_id[0], class_id[1], class_id[2], class_id[3])
	
	if node.is_property_collection:
		print """\t\t\t\t<properties>"""
		for property in node.properties:
			# Skip the "name" property, which is a special-case ...
			if property.name == "name":
				continue

			property_description = ""
			property_value = ""

			if property.is_enumeration:
				property_description += "Type: enumeration Values: %s" % str(property.enumeration_values)
				property_description = property_description.replace("--", "")
			else:
				property_description += "Type: %s" % property.type

			if property.units:
				property_description += " Units: %s" % property.units

			property_description += " Description: %s" % property.description

			if property.type == "int":
				property_value = str(property.value)
			elif property.type == "bool":
				property_value = str(property.value)
			elif property.type == "double":
				property_value = str(property.value)
			elif property.type == "long":
				property_value = str(property.value)
			elif property.type == "unsigned long":
				property_value = str(property.value)
			elif property.type == "std::string":
				property_value = str(property.value)
			elif property.type == "boost::filesystem::path":
				property_value = str(property.value)
			elif property.type == "k3d::color":
				property_value = str(property.value.red) + " " + str(property.value.green) + " " + str(property.value.blue)
			elif property.type == "k3d::point3":
				property_value = str(property.value[0]) + " " + str(property.value[1]) + " " + str(property.value[2])
			elif property.type == "k3d::vector3":
				property_value = str(property.value[0]) + " " + str(property.value[1]) + " " + str(property.value[2])
			elif property.type == "k3d::normal3":
				property_value = str(property.value[0]) + " " + str(property.value[1]) + " " + str(property.value[2])
			elif property.type == "k3d::angle_axis":
				property_value = str(property.value.angle) + " " + str(property.value.axis[0]) + " " + str(property.value.axis[1]) + " " + str(property.value.axis[2])
			elif property.type == "k3d::bitmap*":
				property_value = ""
			elif property.type == "k3d::mesh*":
				property_value = ""
			elif property.type == "k3d::mesh_selection":
				property_value = ""
			elif property.type == "k3d::bounding_box3":
				property_value = "0 1 2 3 4 5"
			elif property.type == "k3d::matrix4":
				property_value = "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15"
			elif property.type == "k3d::inode*":
				property_value = "0"
			elif property.type == "std::vector<k3d::point3>":
				property_value = ""
			else:
				sys.stderr.write("unknown property type: " + property.type + "\n")
				sys.stderr.flush()

			print """\t\t\t\t\t<!-- %s -->""" % property_description
			print """\t\t\t\t\t<property name=\"%s\">%s</property>""" % (property.name, property_value)


		print """\t\t\t\t</properties>"""

	print """\t\t\t</node>"""

	unique_id += 1

k3d.application.close_document(doc)

print """\t\t</nodes>
\t</document>
</k3dml>"""

sys.stdout.flush()

