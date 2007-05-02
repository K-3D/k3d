#python

import k3d

k3d.log_critical("This is a critical message!")
k3d.log_error("This is an error message!")
k3d.log_warning("This is a warning message!")
k3d.log_info("This is an informational message!")
k3d.log_debug("This is a debug message!")

k3d.ui().message("Wrote several messages to the K-3D log ... check your console output, or use Help > Open Log Window to see them.")
