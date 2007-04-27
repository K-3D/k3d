#python

import k3d

k3d.application.get_command_node("/document/window/menus/assign_hotkeys").execute_command("activate", "")
k3d.application.get_command_node("/assign_hotkeys/close").execute_command("activate", "")
