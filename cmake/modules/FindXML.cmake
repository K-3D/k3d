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

ENDIF(UNIX)

######################################################################
# Win32 specific configuration

IF(WIN32)
	ADD_DEFINITIONS(-DK3D_HAVE_LIBXML2)
	FIND_PATH(K3D_XML_INCLUDE_DIR libxml
		c:/gtk/include/libxml2
		${K3D_GTK_DIR}/include/libxml2
		DOC "Directory where the libxml2 header files are located"
		)
	MARK_AS_ADVANCED(K3D_XML_INCLUDE_DIR)

	SET(K3D_XML_LIB xml2 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_XML_LIB)

ENDIF(WIN32)

######################################################################
# Mac OS X specific configuration

IF(APPLE)
ENDIF(APPLE)

SET(K3D_XML_FOUND 1)

