#python

import k3d
import sys
import os

doc = k3d.new_document()

# Used to sort plugins by-name ...
def plugin_sort(lhs, rhs):
	if lhs.name() < rhs.name():
		return -1
	elif lhs.name() == rhs.name():
		return 0
	return 1

# Generate a mapping of categories to plugins ...
categories = { "All" : [], "Stable" : [], "Experimental" : [], "Deprecated" : [] }

for plugin in k3d.plugin.factory.lookup():
	if plugin.metadata().has_key("k3d:disable-documentation"):
		continue

	categories["All"].append(plugin)

	if plugin.quality() == "stable":
		categories["Stable"].append(plugin)
	elif plugin.quality() == "experimental":
		categories["Experimental"].append(plugin)
	elif plugin.quality() == "deprecated":
		categories["Deprecated"].append(plugin)

	for category in plugin.categories():
		if category not in categories:
			categories[category] = []
		categories[category].append(plugin)

# Create reference documentation for all plugins ...
print """Generating plugin reference documentation ..."""
reference = file("@CMAKE_CURRENT_BINARY_DIR@/plugins.txt", "w")
reference.write("""// Machine-generated file, do not edit by hand!\n\n\n""")
reference.write("""= Plugin Reference Documentation =\n""")

for plugin in sorted(k3d.plugin.factory.lookup(), plugin_sort):
	if plugin.metadata().has_key("k3d:disable-documentation"):
		continue

	print """Creating reference documentation for """ + plugin.name() + """ ..."""

	reference.write("""\n== """ + plugin.name() + """ ==\n""")

	plugin_quality = ""
	if plugin.quality() == "stable":
		plugin_quality = "Stable"
	elif plugin.quality() == "experimental":
		plugin_quality = "Experimental"
	elif plugin.quality() == "deprecated":
		plugin_quality = "Deprecated"

	reference.write("\n.Description\n")
	reference.write(plugin.short_description() + "\n")
	
#	if os.path.exists("@share_SOURCE_DIR@/ngui/rasterized/" + plugin.name() + ".png"):
#		reference.write("|[[Image:" + plugin.name() + ".png]]\n")

	reference.write("""\n.Plugin Status\n""")
	reference.write(plugin_quality + "\n")

	reference.write("""\n.Categories\n""")
	reference.write("""All Plugins""")
	reference.write(""", """ + plugin_quality + " Plugins""")
	for category in plugin.categories():
		reference.write(""", """ + category + " Plugins""")
	reference.write("""\n""")

	if plugin.is_document_plugin():
		node = k3d.plugin.create(plugin, doc)

		if node:
			if len(node.properties()) > 1:
				reference.write("\n." + plugin.name() + " Properties\n")

				reference.write("""[width="80%",options="header"]\n""")
				reference.write("""|=========================================================\n""")
				reference.write("""|Label |Description |Type |Script Name\n""")

				for property in node.properties():
					# Skip the "name" property, which is a special-case ...
					if property.name() == "name":
						continue

					reference.write("|" + property.label() + " ")
					reference.write("|" + property.description() + " ")
					reference.write("|" + property.type() + " ")
					reference.write("|" + property.name() + " ")
					reference.write("\n")

				reference.write("""|=========================================================\n""")

	if len(plugin.metadata()):
		reference.write("\n." + plugin.name() + " Metadata\n")
		reference.write("""[width="80%",options="header"]\n""")
		reference.write("""|=========================================================\n""")
		reference.write("""|Name |Value\n""")

		for name in plugin.metadata():
			reference.write("|" + name + " ")
			reference.write("|" + plugin.metadata()[name] + " \n")

		reference.write("""|=========================================================\n""")

reference.write("""// Machine-generated file, do not edit by hand!\n\n\n""")

## Create an article listing every plugin category ...
#print """Creating plugin categories article ..."""
#article = file("@CMAKE_CURRENT_BINARY_DIR@/wikitext/categories/Plugin Categories", "w")
#article.write("""<!-- Machine-generated file, do not edit by hand! -->\n""")
#
#article.write("""<table border="1" cellpadding="5" cellspacing="0">\n""")
#article.write("""<tr><th>Plugin Categories</th></tr>\n""")
#
#for category in sorted(categories.keys()):
#	article.write("""<tr><td>[[""" + category + " Plugins]]</td></tr>\n""")
#
#article.write("""<table>\n""")
#
#article.write("""<!-- Machine-generated file, do not edit by hand! -->\n""")
#
## Create an article for each plugin category ...
#for category in sorted(categories.keys()):
#	print """Creating plugin category article """ + category + """ ..."""
#	article = file("@CMAKE_CURRENT_BINARY_DIR@/wikitext/categories/" + category + " Plugins", "w")
#	article.write("<!-- Machine-generated file, do not edit by hand! -->\n")
#
#	article.write("""<table border="0" cellpadding="5" cellspacing="0">\n""")
#	article.write("""<tr><td><b>Category:</b></td><td>""" + category + """</td></tr>\n""")
#	article.write("""<tr><td><b>Plugins:</b></td><td>""" + str(len(categories[category])) + """</td></tr>\n""")
#	article.write("""</table>\n""")
#
#	article.write("""<table border="1" cellpadding="5" cellspacing="0">\n""")
#	article.write("""<tr><th>Icon</th><th>Plugin</th><th>Description</th></tr>\n""")
#
#	for plugin in sorted(categories[category], plugin_sort):
#		article.write("""<tr>""")
#
#		if os.path.exists("@share_SOURCE_DIR@/ngui/rasterized/" + plugin.name() + ".png"):
#			article.write("""<td>[[Image:""" + plugin.name() + """.png]]</td>""")
#		else:
#			article.write("""<td>&nbsp;</td>""")
#	
#		article.write("""<td>[[""" + plugin.name() + """]]</td><td>""" + plugin.short_description() + """</td></tr>\n""")
#
#	article.write("""</table>\n""")
#
#	article.write("<!-- Machine-generated file, do not edit by hand! -->\n")
#

k3d.close_document(doc)

sys.stdout.flush()

