#python

import k3d

k3d.get_command_node("/document/window/panel2").execute_command("mount", "materialManager")
k3d.get_command_node("/document/window/panel2").execute_command("mount", "NGUIViewportPanel")
