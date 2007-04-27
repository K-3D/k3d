######################################################################
# Posix specific configuration

IF(UNIX)
	# Configure gtkglext ...
	SET(K3D_GDKGLEXT_LIB gdkglext-x11-1.0 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GDKGLEXT_LIB)

	SET(K3D_GTKGLEXT_LIB gtkglext-x11-1.0 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GTKGLEXT_LIB)

ENDIF(UNIX)

######################################################################
# Win32 specific configuration

IF(WIN32)
	# Configure gtkglext ...
	SET(K3D_GDKGLEXT_LIB gdkglext-win32-1.0 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GDKGLEXT_LIB)

	SET(K3D_GTKGLEXT_LIB gtkglext-win32-1.0 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GTKGLEXT_LIB)

ENDIF(WIN32)

######################################################################
# Mac OS X specific configuration

IF(APPLE)
ENDIF(APPLE)

######################################################################
# Platform-independent configuration

# Configure gtkglext ...
FIND_PATH(K3D_GTKGLEXT_CONFIG_INCLUDE_DIR gdkglext-config.h
	/usr/lib/gtkglext-1.0/include
	c:/gtk/lib/gtkglext-1.0/include
	DOC "Directory where the gtkglext config file is located"
	)
MARK_AS_ADVANCED(K3D_GTKGLEXT_CONFIG_INCLUDE_DIR)

FIND_PATH(K3D_GTKGLEXT_INCLUDE_DIR gtk
	/usr/include/gtkglext-1.0
	c:/gtk/include/gtkglext-1.0
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

