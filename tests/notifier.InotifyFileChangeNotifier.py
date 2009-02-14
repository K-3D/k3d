#python

import k3d
import testing

receiver = k3d.file_change_receiver()
notifier = k3d.plugin.create("InotifyFileChangeNotifier")

path = k3d.generic_path(testing.binary_path() + "/" + "notifier.InotifyFileChangeNotifier.test")

f = file(str(path), "w")
f.write("Howdy, ")
f.close()

id1 = notifier.watch_file(path, receiver)
if not id1:
	raise Exception("couldn't create 1st watch")

id2 = notifier.watch_file(path, receiver)
if not id2:
	raise Exception("couldn't create 2nd watch")

notifier.unwatch_file(id1)
notifier.unwatch_file(id2)

id3 = notifier.watch_file(path, receiver)
if not id3:
	raise Exception("couldn't create 3rd watch")

if notifier.change_count():
	raise Exception("unexpected changes pending")

f = file(str(path), "w")
f.write("World!\n")
f.close()

if not notifier.change_count():
	raise Exception("missing changes pending")

while notifier.change_count():
	notifier.signal_change()

changed = receiver.changed()
if len(changed) != 1:
	raise Exception("expected one change")

if changed[0] != path:
	raise Exception("unexpected modification to " + str(changed[0]))

receiver.clear()
changed = receiver.changed()
if len(changed) != 0:
	raise Exception("expected zero changes")

