#python

import k3d

k3d.get_command_node("/document/window/toolbar/create/create_PolyCube").execute_command("activate", "")
k3d.get_command_node("/document/window/menus/mesh_modifier_BevelPoints").execute_command("activate", "")
k3d.get_command_node("/document/window/node_properties/offset").execute_command("set_value", "0.4")
