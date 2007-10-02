SET(K3D_XML_FOUND)

######################################################################
# Posix specific configuration

IF(UNIX)
	ADD_DEFINITIONS(-DK3D_HAVE_EXPAT)
	FIND_PATH(K3D_XML_INCLUDE_DIR expat.h
		/usr/include
		DOC "Directory where the expat header files are located"
		)
	MARK_AS_ADVANCED(K3D_XML_INCLUDE_DIR)

	SET(K3D_XML_LIB expat CACHE STRING "")
	MARK_AS_ADVANCED(K3D_XML_LIB)

	SET(K3D_XML_FOUND 1)
ENDIF(UNIX)

######################################################################
# Win32 specific configuration

IF(WIN32)
	INCLUDE(FindPkgConfig)
	PKG_CHECK_MODULES(XML libxml-2.0)

	IF(XML_FOUND)
		SET(K3D_XML_INCLUDE_DIR
			${XML_INCLUDE_DIRS}
			)

		SET(K3D_XML_LIB_DIRS
			${XML_LIBRARY_DIRS}
			)

		SET(K3D_XML_LIB
			${XML_LIBRARIES}
			)

		ADD_DEFINITIONS(-DK3D_HAVE_LIBXML2)

		SET(K3D_XML_FOUND 1)
	ENDIF(XML_FOUND)

ENDIF(WIN32)

