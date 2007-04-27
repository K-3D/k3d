#python

import k3d

k3d.application.get_command_node("/document/window/menus/create_BitmapChecker").execute_command("activate", "")
k3d.application.get_command_node("/document/window/menus/create_BitmapSubtract").execute_command("activate", "")
k3d.application.get_command_node("/document/window/node_properties/input_bitmap_property").execute_command("connect_to", "<arguments><node>0x8d65a65f 0x853e4460 0xbc2ecbad 0xce65b404 1</node><property>output_bitmap</property></arguments>")
