#python
import k3d
import testing

#Create a polycube
k3d.atk.root().frame("Untitled Document 1").filler("vbox").panel("main").split_pane("main").panel("NGUIToolbarPanel").filler(0).panel(0).filler(0).filler(0).page_tab_list(0).page_tab("Create").filler(0).filler(0).push_button(0).click()

# Switch to point selection mode
k3d.atk.root().frame("Untitled Document 1").filler("vbox").panel("main").split_pane("main").panel("NGUIToolbarPanel").filler("vbox").panel("content").filler("vbox").filler("toolbox").filler("main_toolbar").filler("row1").toggle_button("Select Nodes").click()
k3d.atk.root().frame("Untitled Document 1").filler("vbox").panel("main").split_pane("main").panel("NGUIToolbarPanel").filler("vbox").panel("content").filler("vbox").filler("toolbox").filler("main_toolbar").filler("row1").toggle_button("Select Points").click()

doc = k3d.documents()[0]
cube_instance = doc.get_node("PolyCube Instance")

# set the selection on the PolyCube instance
selection = k3d.geometry.selection.create(0)
point_selection = k3d.geometry.point_selection.create(selection)
k3d.geometry.point_selection.append(point_selection, 0, 4, 1)
cube_instance.mesh_selection = selection

# Switch to move tool
k3d.atk.root().frame("Untitled Document 1").filler("vbox").panel("main").split_pane("main").panel("NGUIToolbarPanel").filler("vbox").panel("content").filler("vbox").filler("toolbox").filler("main_toolbar").filler("row0").toggle_button("selection_tool").click()
k3d.atk.root().frame("Untitled Document 1").filler("vbox").panel("main").split_pane("main").panel("NGUIToolbarPanel").filler("vbox").panel("content").filler("vbox").filler("toolbox").filler("main_toolbar").filler("row0").toggle_button("move_tool").click()

# Activate the move tool properties and change the parameters
k3d.atk.root().frame("Untitled Document 1").filler("vbox").panel("main").split_pane("main").split_pane("bottom").split_pane("top").split_pane("left").split_pane("bottom").panel("NGUINodePropertiesPanel").filler("vbox").filler("decorations").combo_box("panel_type").menu(0).menu_item("NGUIToolPropertiesPanel").click()
k3d.atk.root().frame("Untitled Document 1").filler("vbox").panel("main").split_pane("main").split_pane("bottom").split_pane("top").split_pane("left").split_pane("bottom").panel("NGUIToolPropertiesPanel").filler("vbox").panel("content").filler(0).filler("vbox").scroll_pane(0).viewport(0).filler("move_tool").panel("Builtin Properties").panel(0).panel("move_control").panel(2).push_button("up").click()
k3d.atk.root().frame("Untitled Document 1").filler("vbox").panel("main").split_pane("main").split_pane("bottom").split_pane("top").split_pane("left").split_pane("bottom").panel("NGUIToolPropertiesPanel").filler("vbox").panel("content").filler(0).filler("vbox").scroll_pane(0).viewport(0).filler("move_tool").panel("Builtin Properties").panel(0).panel("move_control").panel(2).push_button("up").click()
k3d.atk.root().frame("Untitled Document 1").filler("vbox").panel("main").split_pane("main").split_pane("bottom").split_pane("top").split_pane("left").split_pane("bottom").panel("NGUIToolPropertiesPanel").filler("vbox").panel("content").filler(0).filler("vbox").scroll_pane(0).viewport(0).filler("move_tool").panel("Builtin Properties").panel(0).panel("move_control").panel(2).push_button("up").click()
k3d.atk.root().frame("Untitled Document 1").filler("vbox").panel("main").split_pane("main").split_pane("bottom").split_pane("top").split_pane("left").split_pane("bottom").panel("NGUIToolPropertiesPanel").filler("vbox").panel("content").filler(0).filler("vbox").scroll_pane(0).viewport(0).filler("move_tool").panel("Builtin Properties").panel(0).panel("move_control").panel(2).push_button("up").click()
k3d.atk.root().frame("Untitled Document 1").filler("vbox").panel("main").split_pane("main").split_pane("bottom").split_pane("top").split_pane("left").split_pane("bottom").panel("NGUIToolPropertiesPanel").filler("vbox").panel("content").filler(0).filler("vbox").scroll_pane(0).viewport(0).filler("move_tool").panel("Builtin Properties").panel(0).panel("move_control").panel(2).push_button("up").click()

tweakpoints = doc.get_node("Move PolyCube Instance components")
testing.mesh_reference_comparison(doc, tweakpoints.get_property("output_mesh"), "ngui.tools.move", 1)
