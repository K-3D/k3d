#python

import k3d

# While recording state changes for undo/redo purposes, you must ensure that errors are handled properly!

Document.start_change_set()
try:
	if k3d.ui.query_message("Choose one ...", ["Succeed", "Fail"]) == 2:
		raise RuntimeError("Simulated error condition")

	Document.finish_change_set("Test Change Set")
	k3d.ui.message("State recording completed successfully ... check your Undo Tree Panel")

except:
	Document.cancel_change_set()
	k3d.ui.message("State recording cancelled due to an error!")
