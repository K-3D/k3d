#python
import k3d
import glob
import os
import testing

factories = []
for factory in k3d.plugin.factory.lookup():
	factories.append(factory.name())

scalable_icons = []
for icon in glob.glob(str(k3d.share_path() / k3d.filesystem.generic_path("ngui/scalable/*.svg"))):
	scalable_icons.append(os.path.splitext(os.path.basename(icon))[0])

factories.sort()
scalable_icons.sort()

exceptions = ['BevelFaces', 'BitmapSequenceIn', 'BitmapSequenceOut', 'EXRBitmapExporter', 'ImageMagickBitmapExporter', 'Viewport', 'cant_connect_property_cursor', 'center', 'connect_property_cursor', 'connected_plug', 'convert_selection', 'element', 'keep_selection', 'knife_cursor', 'move_cursor', 'move_cursor_screen_xy', 'move_cursor_x', 'move_cursor_xy', 'move_cursor_xz', 'move_cursor_y', 'move_cursor_yz', 'move_cursor_z', 'move_tool', 'node_history_panel', 'node_list_panel', 'node_properties_panel', 'parent_cursor', 'plug_tool', 'render_animation', 'render_frame', 'render_preview', 'rotate_tool', 'scale_cursor', 'scale_cursor_x', 'scale_cursor_xy', 'scale_cursor_xyz', 'scale_cursor_xz', 'scale_cursor_y', 'scale_cursor_yz', 'scale_cursor_z', 'scale_tool', 'select_add_cursor', 'select_cursor', 'select_curve', 'select_face', 'select_group', 'select_node', 'select_patch', 'select_split_edge', 'select_subtract_cursor', 'select_tool', 'select_uniform', 'select_vertex', 'soft_selection', 'undo_tree_panel', 'unparent', 'viewport_panel']

unexpected_icons = []
for icon in scalable_icons:
	if icon not in factories:
		if icon in exceptions:
			continue
		unexpected_icons.append(icon)

if len(unexpected_icons):
	print "Unexpected icons without factories:", unexpected_icons
	raise Exception("Found unexpected icons without corresponding factories.")

