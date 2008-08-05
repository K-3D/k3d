SET(K3D_3DS_FOUND FALSE)

#Unix configuration
IF(UNIX AND NOT APPLE)
	FIND_PATH(K3D_3DS_INCLUDE_PATH /lib3ds
		/usr/local/include
		/usr/include
		DOC "Directory where the lib3ds header files are located"
		)
	
	FIND_LIBRARY(K3D_3DS_LIBRARY
		NAMES 3ds
		PATHS
		/usr/local/lib
		/usr/lib
		DOC "The lib3ds library"
		)
	
ENDIF(UNIX AND NOT APPLE)
	
#Apple configuration
IF(UNIX AND APPLE)
	FIND_PATH(K3D_COLLADA_DOM_INCLUDE_PATH /lib3ds
		/Library/Frameworks/lib3ds.framework
		DOC "Directory where the lib3ds header files are located"
		)

	FIND_LIBRARY(K3D_3DS_LIBRARY
		NAMES lib3ds
		PATHS
		/Library/Frameworks/lib3ds.framework
		DOC "The lib3ds library"
		)

ENDIF(UNIX AND APPLE)

#Windows configuration
IF(WIN32)
	FIND_PATH(K3D_3DS_INCLUDE_PATH lib3ds/
		C:/Program Files/lib3ds/include
		DOC "Directory where the lib3ds header files are located"
		)
	
	FIND_LIBRARY(K3D_3DS_LIBRARY
		NAMES lib3ds
		PATHS
		C:/Program Files/lib3ds/lib/
		DOC "The lib3ds library"
		)
ENDIF(WIN32)

MARK_AS_ADVANCED(K3D_3DS_INCLUDE_PATH)	
MARK_AS_ADVANCED(K3D_3DS_LIBRARY)

SET(K3D_3DS_INCLUDE_DIRS ${K3D_3DS_INCLUDE_PATH})
SET(K3D_3DS_LIBS ${K3D_3DS_LIBRARY})

IF(WIN32)
    SET(K3D_3DS_LIBS ${K3D_3DS_LIBS} pcrecpp pcre)
ENDIF(WIN32)

IF(K3D_3DS_INCLUDE_PATH AND K3D_3DS_LIBRARY)
	SET(K3D_3DS_FOUND TRUE)
ENDIF(K3D_3DS_INCLUDE_PATH AND K3D_3DS_LIBRARY)

