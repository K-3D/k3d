#python

import k3d
import sys
import os

doc = k3d.new_document()

plugins = {}
for plugin in k3d.plugins():
	plugins[plugin.name] = plugin

sorted_plugins = plugins.keys()
sorted_plugins.sort()

for plugin_name in sorted_plugins:
	plugin = plugins[plugin_name]

	class_id = plugin.class_id

	plugin_name = plugin.name
	
	plugin_description = plugin.short_description
	
	plugin_quality = ""
	if plugin.quality == "stable":
		plugin_quality = "Stable"
	elif plugin.quality == "experimental":
		plugin_quality = "Experimental"
	elif plugin.quality == "deprecated":
		plugin_quality = "Deprecated"

	overview = file("docs/wiki/" + plugin_name + ".wiki", "w")
	overview.write("{{Plugin/" + plugin_name + "}}\n")

	detail = file("docs/wiki/Template:Plugin/" + plugin_name + ".wiki", "w")
	detail.write("<!-- Machine-generated file, do not edit by hand! -->\n")

	detail.write("== Description == " + "\n")
	
	detail.write("{| border=\"0\" cellpadding=\"5\" cellspacing=\"0\"\n")
	detail.write("|-\n")

	if os.path.exists("share/ngui/rasterized/" + plugin_name + ".png"):
		detail.write("|[[Image:" + plugin_name + ".png]]\n")
	
	detail.write("|" + plugin_description + "\n")
	detail.write("|}\n")
	
	detail.write("== Status == " + "\n")
	detail.write("[[Plugin Status|" + plugin_quality + "]]\n")

	if plugin.is_document_plugin:
		node = doc.new_node(plugin.name)
		if node.is_property_collection:

			detail.write("== Properties == " + "\n")

			detail.write("{| border=\"1\" cellpadding=\"5\" cellspacing=\"0\"\n")
			detail.write("! Label\n")
			detail.write("! Description\n")
			detail.write("! Type\n")
			detail.write("! Script Name\n")

			for property in node.properties:
				# Skip the "name" property, which is a special-case ...
				if property.name == "name":
					continue

				detail.write("|-\n")
				detail.write("|'''" + property.label + "'''\n")
				detail.write("|" + property.description + "\n")
				detail.write("|[[Property Types#" + property.type + "|" + property.type + "]]\n")
				detail.write("|" + property.name + "\n")

			detail.write("|}\n")

	detail.write("[[Category:Plugins]]\n")
	detail.write("[[Category:" + plugin_quality + "]]\n")
	for category in plugin.categories:
		detail.write("[[Category:" + category + "]]\n")

	detail.write("<!-- Machine-generated file, do not edit by hand! -->\n")

k3d.close_document(doc)

sys.stdout.flush()

