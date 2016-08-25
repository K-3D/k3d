SET(K3D_COLLADA_FOUND FALSE)

IF(NOT APPLE)
	FIND_PATH(K3D_COLLADA_BASE_INCLUDE_PATH 1.4
		/usr/include/collada-dom2.4
		/usr/include/colladadom
		/usr/local/include/collada-dom2.4
		/usr/local/include/colladadom
		DOC "Directory where the collada includes are located"
		)

    SET(K3D_COLLADA_DAE_INCLUDE_PATH ${K3D_COLLADA_BASE_INCLUDE_PATH} CACHE PATH
	"Directoy containing the Collada dae.h include file")

	SET(K3D_COLLADA_DOM_INCLUDE_PATH ${K3D_COLLADA_BASE_INCLUDE_PATH}/1.4 CACHE PATH
	"Directoy containing the Collada dom includes")

	FIND_LIBRARY(K3D_COLLADA_LIBRARY
		NAMES collada14dom collada-dom2.4-dp collada-dom2.4
		PATHS
		/usr/local/lib
		/usr/lib
		DOC "The ColladaDOM 1.4 library"
		)
ENDIF(NOT APPLE)

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

MARK_AS_ADVANCED(K3D_COLLADA_BASE_INCLUDE_PATH)
MARK_AS_ADVANCED(K3D_COLLADA_DAE_INCLUDE_PATH)
MARK_AS_ADVANCED(K3D_COLLADA_DOM_INCLUDE_PATH)
MARK_AS_ADVANCED(K3D_COLLADA_LIBRARY)

SET(K3D_COLLADA_INCLUDE_DIRS ${K3D_COLLADA_DAE_INCLUDE_PATH} ${K3D_COLLADA_DOM_INCLUDE_PATH})
SET(K3D_COLLADA_LIBS ${K3D_COLLADA_LIBRARY})

IF(K3D_COLLADA_DAE_INCLUDE_PATH AND K3D_COLLADA_DOM_INCLUDE_PATH AND K3D_COLLADA_LIBRARY)
	SET(K3D_COLLADA_FOUND TRUE)
ENDIF(K3D_COLLADA_DAE_INCLUDE_PATH AND K3D_COLLADA_DOM_INCLUDE_PATH AND K3D_COLLADA_LIBRARY)
