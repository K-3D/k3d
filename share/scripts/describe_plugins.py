#python

import k3d

factories = k3d.plugins()

application_plugins = []
document_plugins = []

for factory in factories :
	if factory.is_application_plugin():
		application_plugins.append(factory.name())
	if factory.is_document_plugin():
		document_plugins.append(factory.name())

application_plugins.sort()
document_plugins.sort()

print "\nApplication Plugins:"
for plugin in application_plugins:
	print "\t" + plugin
print "\n\t" + "Total: " + str(len(application_plugins))

print "\nDocument Plugins:"
for plugin in document_plugins:
	print "\t" + plugin
print "\n\t" + "Total: " + str(len(document_plugins))

import sys
sys.stdout.flush()

k3d.ui.message("Output sent to console")

