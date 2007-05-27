ADD_DEFINITIONS(-DK3D_HAVE_SIGC_2_0)
FIND_PATH(K3D_SIGC_CONFIG_INCLUDE_DIR sigc++config.h
	/usr/lib/sigc++-2.0/include
	/opt/local/lib/sigc++-2.0/include
	c:/gtk/lib/sigc++-2.0/include
	DOC "Directory where the sigc++ config file is located"
	)
MARK_AS_ADVANCED(K3D_SIGC_CONFIG_INCLUDE_DIR)

FIND_PATH(K3D_SIGC_INCLUDE_DIR sigc++
	/usr/include/sigc++-2.0
	/opt/local/include/sigc++-2.0
	c:/gtk/include/sigc++-2.0
	DOC "Directory where the sigc++ header files are located"
	)
MARK_AS_ADVANCED(K3D_SIGC_INCLUDE_DIR)

SET(K3D_SIGC_LIB sigc-2.0 CACHE STRING "")
MARK_AS_ADVANCED(K3D_SIGC_LIB)

# Following are the consolidated variables that should be used for builds
SET(K3D_SIGC_INCLUDE_DIRS
	${K3D_SIGC_CONFIG_INCLUDE_DIR}
	${K3D_SIGC_INCLUDE_DIR}
	)

SET(K3D_SIGC_LIB_DIRS
	${K3D_GTK_LIB_DIR}
	)

SET(K3D_SIGC_FOUND 1)

