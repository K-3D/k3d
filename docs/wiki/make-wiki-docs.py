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

for plugin in k3d.plugins():
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

# Create the main article for each plugin ...
for plugin in sorted(k3d.plugins(), plugin_sort):
	print """Creating main article for """ + plugin.name() + """ ..."""
	article = file("@CMAKE_CURRENT_BINARY_DIR@/wikitext/plugins/articles/" + plugin.name(), "w")
	article.write("<plugin>{{PAGENAME}}</plugin>\n")

# Create an article listing every plugin category ...
print """Creating plugin categories article ..."""
article = file("@CMAKE_CURRENT_BINARY_DIR@/wikitext/plugins/categories/Plugin Categories", "w")
article.write("""<!-- Machine-generated file, do not edit by hand! -->\n""")

article.write("""<table border="1" cellpadding="5" cellspacing="0">\n""")
article.write("""<tr><th>Plugin Categories</th></tr>\n""")

for category in sorted(categories.keys()):
	article.write("""<tr><td>[[""" + category + " Plugins]]</td></tr>\n""")

article.write("""<table>\n""")

article.write("""<!-- Machine-generated file, do not edit by hand! -->\n""")

# Create an article for each plugin category ...
for category in sorted(categories.keys()):
	print """Creating plugin category article """ + category + """ ..."""
	article = file("@CMAKE_CURRENT_BINARY_DIR@/wikitext/plugins/categories/" + category + " Plugins", "w")
	article.write("<!-- Machine-generated file, do not edit by hand! -->\n")

	article.write("""<table border="0" cellpadding="5" cellspacing="0">\n""")
	article.write("""<tr><td><b>Category:</b></td><td>""" + category + """</td></tr>\n""")
	article.write("""<tr><td><b>Plugins:</b></td><td>""" + str(len(categories[category])) + """</td></tr>\n""")
	article.write("""</table>\n""")

	article.write("""<table border="1" cellpadding="5" cellspacing="0">\n""")
	article.write("""<tr><th>Icon</th><th>Plugin</th><th>Description</th></tr>\n""")

	for plugin in sorted(categories[category], plugin_sort):
		article.write("""<tr>""")

		if os.path.exists("@share_SOURCE_DIR@/ngui/rasterized/" + plugin.name() + ".png"):
			article.write("""<td>[[Image:""" + plugin.name() + """.png]]</td>""")
		else:
			article.write("""<td>&nbsp;</td>""")
	
		article.write("""<td>[[""" + plugin.name() + """]]</td><td>""" + plugin.short_description() + """</td></tr>\n""")

	article.write("""</table>\n""")

	article.write("<!-- Machine-generated file, do not edit by hand! -->\n")

# Create the reference documentation for each plugin ...
for plugin in sorted(k3d.plugins(), plugin_sort):

	print """Creating reference documentation for """ + plugin.name() + """ ..."""

	plugin_quality = ""
	if plugin.quality() == "stable":
		plugin_quality = "Stable"
	elif plugin.quality() == "experimental":
		plugin_quality = "Experimental"
	elif plugin.quality() == "deprecated":
		plugin_quality = "Deprecated"

	article = file("@CMAKE_CURRENT_BINARY_DIR@/wikitext/plugins/reference/" + plugin.name(), "w")
	article.write("<!-- Machine-generated file, do not edit by hand! -->\n")

	article.write("== Description == " + "\n")
	
	article.write("{| border=\"0\" cellpadding=\"5\" cellspacing=\"0\"\n")
	article.write("|-\n")

	if os.path.exists("@share_SOURCE_DIR@/ngui/rasterized/" + plugin.name() + ".png"):
		article.write("|[[Image:" + plugin.name() + ".png]]\n")
	
	article.write("|" + plugin.short_description() + "\n")
	article.write("|}\n")
	
	article.write("""<table border="0" cellpadding="5" cellspacing="0">\n""")
	article.write("""<tr><td><b>Plugin Status:</b></td><td>[[Plugin Status|""" + plugin_quality + """]]</td></tr>\n""")

	article.write("""<tr><td><b>Categories:</b></td><td>""")
	article.write("""[[All Plugins]]""")
	article.write(""", [[""" + plugin_quality + " Plugins]]""")
	for category in plugin.categories():
		article.write(""", [[""" + category + " Plugins]]""")
	article.write("""</td></tr>\n""")

	article.write("""</table>\n""")

	if plugin.is_document_plugin():
		node = doc.new_node(plugin.name())

		article.write("== Properties == " + "\n")

		article.write("{| border=\"1\" cellpadding=\"5\" cellspacing=\"0\"\n")
		article.write("! Label\n")
		article.write("! Description\n")
		article.write("! Type\n")
		article.write("! Script Name\n")

		for property in node.properties():
			# Skip the "name" property, which is a special-case ...
			if property.name() == "name":
				continue

			article.write("|-\n")
			article.write("|'''" + property.label() + "'''\n")
			article.write("|" + property.description() + "\n")
			article.write("|[[Property Types#" + property.type() + "|" + property.type() + "]]\n")
			article.write("|" + property.name() + "\n")

		article.write("|}\n")

	article.write("<!-- Machine-generated file, do not edit by hand! -->\n")

k3d.close_document(doc)

sys.stdout.flush()

