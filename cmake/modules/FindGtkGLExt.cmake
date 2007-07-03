SET(K3D_GTKGLEXT_FOUND 0)

######################################################################
# Posix specific configuration

IF(UNIX)
	INCLUDE(FindPkgConfig)
	PKG_CHECK_MODULES(GTKGLEXT gtkglext-1.0)

	IF(GTKGLEXT_FOUND)
		SET(K3D_GTKGLEXT_INCLUDE_DIRS
			${GTKGLEXT_INCLUDE_DIRS}
			)

		SET(K3D_GTKGLEXT_LIB_DIRS
			${GTKGLEXT_LIBRARY_DIRS}
			)

		SET(K3D_GTKGLEXT_LIBS
			${GTKGLEXT_LIBRARIES}
			)

		SET(K3D_GTKGLEXT_FOUND 1)
	ENDIF(GTKGLEXT_FOUND)

ENDIF(UNIX)

######################################################################
# Win32 specific configuration

IF(WIN32)
	# Configure gtkglext ...
	SET(K3D_GDKGLEXT_LIB gdkglext-win32-1.0 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GDKGLEXT_LIB)

	SET(K3D_GTKGLEXT_LIB gtkglext-win32-1.0 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GTKGLEXT_LIB)

	FIND_PATH(K3D_GTKGLEXT_CONFIG_INCLUDE_DIR gdkglext-config.h
		c:/gtk/lib/gtkglext-1.0/include
		${K3D_GTK_DIR}/lib/gtkglext-1.0/include 
		DOC "Directory where the gtkglext config file is located"
		)
	MARK_AS_ADVANCED(K3D_GTKGLEXT_CONFIG_INCLUDE_DIR)

	FIND_PATH(K3D_GTKGLEXT_INCLUDE_DIR gtk
		c:/gtk/include/gtkglext-1.0
		${K3D_GTK_DIR}/include/gtkglext-1.0
		DOC "Directory where the gtkglext header files are located"
		)
	MARK_AS_ADVANCED(K3D_GTKGLEXT_INCLUDE_DIR)

	SET(K3D_GTKGLEXT_INCLUDE_DIRS
		${K3D_GTKGLEXT_CONFIG_INCLUDE_DIR}
		${K3D_GTKGLEXT_INCLUDE_DIR}
		)

	SET(K3D_GTKGLEXT_LIBS
		${K3D_GDKGLEXT_LIB}
		${K3D_GTKGLEXT_LIB}
		)

	SET(K3D_GTKGLEXT_FOUND 1)

ENDIF(WIN32)

