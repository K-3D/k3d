#python

import k3d

factories = k3d.plugins()

k3d.ui().message("K-3D found " + str(len(factories)) + " plugins")

