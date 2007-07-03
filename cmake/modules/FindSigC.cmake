SET(K3D_SIGC_FOUND 0)


######################################################################
# Posix specific configuration

IF(UNIX)
	INCLUDE(FindPkgConfig)
	PKG_CHECK_MODULES(SIGC sigc++-2.0)

	IF(SIGC_FOUND)
		SET(K3D_SIGC_INCLUDE_DIRS
			${SIGC_INCLUDE_DIRS}
			)

		SET(K3D_SIGC_LIB_DIRS
			${SIGC_LIBRARY_DIRS}
			)

		SET(K3D_SIGC_LIBS
			${SIGC_LIBRARIES}
			)

		SET(K3D_SIGC_FOUND 1)

		ADD_DEFINITIONS(-DK3D_HAVE_SIGC_2_0)

	ENDIF(SIGC_FOUND)

ENDIF(UNIX)

IF(WIN32)
	FIND_PATH(K3D_SIGC_CONFIG_INCLUDE_DIR sigc++config.h
		c:/gtk/lib/sigc++-2.0/include
		${K3D_GTK_DIR}/lib/sigc++-2.0/include
		DOC "Directory where the sigc++ config file is located"
		)
	MARK_AS_ADVANCED(K3D_SIGC_CONFIG_INCLUDE_DIR)

	FIND_PATH(K3D_SIGC_INCLUDE_DIR sigc++
		c:/gtk/include/sigc++-2.0
		${K3D_GTK_DIR}/include/sigc++-2.0 
		DOC "Directory where the sigc++ header files are located"
		)
	MARK_AS_ADVANCED(K3D_SIGC_INCLUDE_DIR)

	IF(MSVC)
		SET(K3D_SIGC_LIB optimized sigc-2.0 debug sigc-2.0d CACHE STRING "")
	ELSE(MSVC)
		SET(K3D_SIGC_LIB sigc-2.0 CACHE STRING "")
	ENDIF(MSVC)
	MARK_AS_ADVANCED(K3D_SIGC_LIB)

	# Following are the consolidated variables that should be used for builds
	SET(K3D_SIGC_INCLUDE_DIRS
		${K3D_SIGC_CONFIG_INCLUDE_DIR}
		${K3D_SIGC_INCLUDE_DIR}
		)

	SET(K3D_SIGC_LIB_DIRS
		${K3D_GTK_LIB_DIR}
		)

	SET(K3D_SIGC_LIBS
		${K3D_SIGC_LIB}
		)

	SET(K3D_SIGC_FOUND 1)

	ADD_DEFINITIONS(-DK3D_HAVE_SIGC_2_0)

ENDIF(WIN32)

