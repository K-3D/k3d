#python

import k3d

factories = k3d.plugin.factory.lookup()

k3d.ui().message("There are " + str(len(factories)) + " K-3D plugins installed.")

