#python

import k3d

def innovate():
	global context
	global k3d

	# Give folks a chance to bail ...
	if k3d.ui().query_message("Are you sure?  Don't run this script on a real working document!", ["OK", "Cancel"]) == 2:
		return

	# Start recording changes for undo-purposes ...
	context.document.start_change_set()
	try:
		# Deliver "value" ...
		for node in k3d.node.lookup(context.document):
			node.name = "Microsoft " + node.name + " (TM)"

		# Finish recording undos ...
		context.document.finish_change_set("Innovate!")

		# Communicate the good news to our "customer"!
		k3d.ui().message("You have been Innovated ... check your Node List Panel (it's undo-able)")

	except:
		context.document.cancel_change_set()
		raise

innovate()

