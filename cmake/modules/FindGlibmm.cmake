SET(K3D_GLIBMM_FOUND 0)

######################################################################
# Posix specific configuration

IF(UNIX)
	INCLUDE(FindPkgConfig)
	PKG_CHECK_MODULES(GLIBMM glibmm-2.4)

	IF(GLIBMM_FOUND)
		SET(K3D_GLIBMM_INCLUDE_DIRS
			${GLIBMM_INCLUDE_DIRS}
			)

		SET(K3D_GLIBMM_LIB_DIRS
			${GLIBMM_LIBRARY_DIRS}
			)

		SET(K3D_GLIBMM_LIBS
			${GLIBMM_LIBRARIES}
			)

		SET(K3D_GLIBMM_FOUND 1)
	ENDIF(GLIBMM_FOUND)

ENDIF(UNIX)


######################################################################
# Win32 specific configuration

IF(WIN32)
	# Configure gobject ...
	SET(K3D_GOBJECT_LIB gobject-2.0 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GOBJECT_LIB)

	# Configure glib ...
	FIND_PATH(K3D_GLIB_CONFIG_INCLUDE_DIR glibconfig.h
		c:/gtk/lib/glib-2.0/include
		DOC "Directory where the glib config file is located"
		)
	MARK_AS_ADVANCED(K3D_GLIB_CONFIG_INCLUDE_DIR)

	FIND_PATH(K3D_GLIB_INCLUDE_DIR glib.h
		c:/gtk/include/glib-2.0
		DOC "Directory where the glib header files are located"
		)
	MARK_AS_ADVANCED(K3D_GLIB_INCLUDE_DIR)

	SET(K3D_GLIB_LIB glib-2.0 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GLIB_LIB)

	# Configure glibmm ...
	FIND_PATH(K3D_GLIBMM_CONFIG_INCLUDE_DIR glibmmconfig.h
		c:/gtk/lib/glibmm-2.4/include
		DOC "Directory where the glibmm config file is located"
		)
	MARK_AS_ADVANCED(K3D_GLIBMM_CONFIG_INCLUDE_DIR)

	FIND_PATH(K3D_GLIBMM_INCLUDE_DIR glibmm.h
		c:/gtk/include/glibmm-2.4
		DOC "Directory where the glibmm header files are located"
		)
	MARK_AS_ADVANCED(K3D_GLIBMM_INCLUDE_DIR)

	SET(K3D_GLIBMM_LIB glibmm-2.4 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GLIBMM_LIB)

	# Following are the consolidated variables that should be used for builds
	SET(K3D_GLIBMM_INCLUDE_DIRS
		${K3D_GLIB_CONFIG_INCLUDE_DIR}
		${K3D_GLIB_INCLUDE_DIR}
		${K3D_GLIBMM_CONFIG_INCLUDE_DIR}
		${K3D_GLIBMM_INCLUDE_DIR}
		)

	SET(K3D_GLIBMM_LIB_DIRS
		${K3D_GTK_LIB_DIR}
		)

	SET(K3D_GLIBMM_LIBS
		${K3D_GLIB_LIB}
		${K3D_GLIBMM_LIB}
		${K3D_GOBJECT_LIB}
		)

	SET(K3D_GLIBMM_FOUND 1)

ENDIF(WIN32)

