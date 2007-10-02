SET(K3D_GTKMM_FOUND 0)

######################################################################
# Posix specific configuration

INCLUDE(K3DFindPkgConfig)
PKG_CHECK_MODULES(GTKMM gtkmm-2.4)

IF(GTKMM_FOUND)
	SET(K3D_GTKMM_INCLUDE_DIRS
		${GTKMM_INCLUDE_DIRS}
		)

	SET(K3D_GTKMM_LIB_DIRS
		${GTKMM_LIBRARY_DIRS}
		)

	SET(K3D_GTKMM_LIBS
		${GTKMM_LIBRARIES}
		)

	SET(K3D_GTKMM_FOUND 1)
ENDIF(GTKMM_FOUND)

