#python

Document.start_change_set()

try:
	for node in Document.nodes():
		node.selection_weight = 0.0

	Document.finish_change_set("Deselect All")

except:
	Document.cancel_change_set()

