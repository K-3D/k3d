SET(K3D_OPENEXR_FOUND 0)

######################################################################
# Posix specific configuration

IF(UNIX)
	INCLUDE(FindPkgConfig)
	PKG_CHECK_MODULES(OPENEXR OpenEXR)

	IF(OPENEXR_FOUND)
		SET(K3D_OPENEXR_INCLUDE_DIRS
			${OPENEXR_INCLUDE_DIRS}
			)

		SET(K3D_OPENEXR_LIB_DIRS
			${OPENEXR_LIBRARY_DIRS}
			)

		SET(K3D_OPENEXR_LIBS
			${OPENEXR_LIBRARIES}
			)

		SET(K3D_OPENEXR_FOUND 1)
	ENDIF(OPENEXR_FOUND)

ENDIF(UNIX)

######################################################################
# Win32 specific configuration

IF(WIN32)
ENDIF(WIN32)

######################################################################
# Mac OS X specific configuration

IF(APPLE)
ENDIF(APPLE)

