#python

import k3d
import sys

def describe_plugins(stream):
	factories = k3d.plugins()

	stream.write( "\n\nApplication Plugins:\n" )
	for factory in factories :
		if factory.is_application_plugin :
			stream.write( "\t" + factory.name + "\n")


	stream.write( "\n\nDocument Plugins:\n" )
	for factory in factories :
		if factory.is_document_plugin :
			stream.write( "\t" + factory.name + "\n" )

	stream.write( "\n\n" )

describe_plugins(sys.stdout)
sys.stdout.flush()

k3d.ui.message("Plugin descriptions written to stdout (check your console).")



