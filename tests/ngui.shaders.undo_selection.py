#python

import k3d

k3d.application.get_command_node("/document/window/node_list").execute_command("select", "<arguments><selection><node>b553a3bd d1044c6e b3f21e07 87132c31 1</node></selection><context_menu/></arguments>")
k3d.application.get_command_node("/document/window/node_properties/shader_path").execute_command("browse", "<arguments><reference>relative</reference><relative_path>shaders/surface/k3d_orange.sl</relative_path><root>$K3D_SHARE_PATH</root></arguments>")
k3d.application.get_command_node("/document/window/menus/edit_undo").execute_command("activate", "")
k3d.application.get_command_node("/document/window/menus/edit_redo").execute_command("activate", "")

