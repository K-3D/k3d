#python

import k3d

k3d.ui.message("This is a message!")
k3d.ui.warning_message("This is a warning message!")
k3d.ui.error_message("This is an error message!")

result = k3d.ui.query_message("Pick one!", ["Choice 1", "Choice 2", "Choice 3"])
k3d.ui.message("You chose " + str(result))

result = k3d.ui.get_file_path("read", "test", "Choose file to read (doesn't actually read anything):", "")
k3d.ui.message("You chose " + result)

result = k3d.ui.get_file_path("write", "test", "Choose file to write (doesn't actually write anything):", "")
k3d.ui.message("You chose " + result)

