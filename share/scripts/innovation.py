#python

import k3d

def innovate(doc):
	global k3d

	# Give folks a chance to bail ...
	if k3d.ui.query_message("Are you sure?  Don't run this script on a real working document!", ["OK", "Cancel"]) == 2:
		return

	# Start recording changes for undo-purposes ...
	doc.start_change_set()
	try:
		# Deliver "value" ...
		for node in doc.nodes():
			node.name = "Microsoft " + node.name + " (TM)"

		# Finish recording undos ...
		doc.finish_change_set("Innovate!")

		# Communicate the good news to our "customer"!
		k3d.ui.message("You have been Innovated ... check your Node List Panel (it's undo-able)")

	except:
		doc.cancel_change_set()
		raise

innovate(Document)

