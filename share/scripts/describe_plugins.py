#python

import k3d
import sys

factories = k3d.plugins()

print "\nApplication Plugins:"
for factory in factories :
	if factory.is_application_plugin :
		print "\t" + factory.name


print "\nDocument Plugins:"
for factory in factories :
	if factory.is_document_plugin :
		print "\t" + factory.name

sys.stdout.flush()

k3d.ui.message("Output sent to console")

