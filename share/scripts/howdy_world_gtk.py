#python

import k3d

# Well-behaved scripts never wait for user-input if batch_mode() is enabled ...
if not k3d.batch_mode():
	import pygtk
	pygtk.require('2.0')
	import gtk

	label = gtk.Label("Helloooooo ... from PyGTK!")
	label.set_padding(5, 5)

	dialog = gtk.Dialog("Howdy, World!", None, gtk.DIALOG_MODAL | gtk.DIALOG_NO_SEPARATOR, (gtk.STOCK_OK, gtk.RESPONSE_ACCEPT))
	dialog.vbox.pack_start(label)
	dialog.show_all();
	dialog.run()
	dialog.hide()

