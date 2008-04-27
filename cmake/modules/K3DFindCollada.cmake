SET(K3D_COLLADA_FOUND FALSE)

#Unix configuration
IF(UNIX AND NOT APPLE)
	FIND_PATH(K3D_COLLADA_DAE_INCLUDE_PATH dae.h
		/usr/local/include/colladadom
		/usr/include/colladadom
		DOC "Directory where the ColladaDOM dae.h file is located"
		)

	FIND_PATH(K3D_COLLADA_DOM_INCLUDE_PATH /dom
		/usr/local/include/colladadom/1.4
		/usr/include/colladadom/1.4
		DOC "Directory where the ColladaDOM 1.4 header files are located"
		)
	
	FIND_LIBRARY(K3D_COLLADA_LIBRARY
		NAMES collada14dom
		PATHS
		/usr/local/lib
		/usr/lib
		DOC "The ColladaDOM 1.4 library"
		)
	
ENDIF(UNIX AND NOT APPLE)
	
#Apple configuration
IF(UNIX AND APPLE)
	FIND_PATH(K3D_COLLADA_DAE_INCLUDE_PATH dae.h
		/Library/Frameworks/Collada14Dom.framework
		DOC "Directory where the ColladaDOM dae.h file is located"
		)

	FIND_PATH(K3D_COLLADA_DOM_INCLUDE_PATH dom/
		/Library/Frameworks/Collada14Dom.framework/1.4
		DOC "Directory where the ColladaDOM 1.4 header files are located"
		)

	FIND_LIBRARY(K3D_COLLADA_LIBRARY
		NAMES Collada14Dom
		PATHS
		/Library/Frameworks/Collada14Dom.framework
		DOC "The ColladaDOM 1.4 library"
		)

ENDIF(UNIX AND APPLE)

#Windows configuration
IF(WIN32)
	FIND_PATH(K3D_COLLADA_DAE_INCLUDE_PATH dae.h
		C:\Program Files\domcollada\include
		DOC "Directory where the ColladaDOM dae.h file is located"
		)

	FIND_PATH(K3D_COLLADA_DOM_INCLUDE_PATH dom/
		C:\Program Files\domcollada\include\1.4
		DOC "Directory where the ColladaDOM 1.4 header files are located"
		)
	
	FIND_LIBRARY(K3D_COLLADA_LIBRARY
		NAMES Collada14Dom
		PATHS
		C:\Program Files\domcollada\build\vc8-1.4
		DOC "The ColladaDOM 1.4 library"
		)
ENDIF(WIN32)

MARK_AS_ADVANCED(K3D_COLLADA_DAE_INCLUDE_PATH)
MARK_AS_ADVANCED(K3D_COLLADA_DOM_INCLUDE_PATH)	
MARK_AS_ADVANCED(K3D_COLLADA_LIBRARY)

SET(K3D_COLLADA_INCLUDE_DIRS ${K3D_COLLADA_DAE_INCLUDE_PATH} ${K3D_COLLADA_DOM_INCLUDE_PATH})
SET(K3D_COLLADA_LIBS ${K3D_COLLADA_LIBRARY})

IF(K3D_COLLADA_DAE_INCLUDE_PATH AND K3D_COLLADA_DOM_INCLUDE_PATH AND K3D_COLLADA_LIBRARY)
	SET(K3D_COLLADA_FOUND TRUE)
ENDIF(K3D_COLLADA_DAE_INCLUDE_PATH AND K3D_COLLADA_DOM_INCLUDE_PATH AND K3D_COLLADA_LIBRARY)

