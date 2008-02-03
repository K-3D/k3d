#python

# k3d:plugin-class="application"
# k3d:plugin-type="ActionScript"
# k3d:plugin-name="Toggle Backfacing"
# ngui:action=""

import k3d

k3d.get_command_node("/document/selection_tool").execute_command("toggle_pick_backfacing", "")
k3d.get_command_node("/document/selection_tool").execute_command("toggle_paint_backfacing", "")
k3d.get_command_node("/document/selection_tool").execute_command("toggle_rubber_band_backfacing", "")
