#python

Document.start_change_set()

try:
  selection_node = Document.get_node_by_metadata("inode_selection", "ngui:unique_node", "node_selection")
  selection_node.deselect_all()

  Document.finish_change_set("Deselect All")

except:
	Document.cancel_change_set()
