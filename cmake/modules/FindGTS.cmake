SET(K3D_GTS_FOUND 0)

######################################################################
# Posix specific configuration

IF(UNIX)
	INCLUDE(FindPkgConfig)
	PKG_CHECK_MODULES(GTS gts)

	IF(GTS_FOUND)
		SET(K3D_GTS_INCLUDE_DIRS
			${GTS_INCLUDE_DIRS}
			)

		SET(K3D_GTS_LIB_DIRS
			${GTS_LIBRARY_DIRS}
			)

		SET(K3D_GTS_LIBS
			${GTS_LIBRARIES}
			)

		SET(K3D_GTS_FOUND 1)
	ENDIF(GTS_FOUND)

ENDIF(UNIX)

######################################################################
# Win32 specific configuration

IF(WIN32)
ENDIF(WIN32)

######################################################################
# Mac OS X specific configuration

IF(APPLE)
ENDIF(APPLE)

