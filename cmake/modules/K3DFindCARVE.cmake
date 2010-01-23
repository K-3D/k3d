SET(K3D_CARVE_FOUND 0)

FIND_PATH(K3D_CARVE_INCLUDE_DIR carve
	/usr/include
	/usr/local/include
	DOC "Directory where the carve directory, containing the headers, is located"
	)
MARK_AS_ADVANCED(K3D_CARVE_INCLUDE_DIR)

FIND_LIBRARY(K3D_CARVE_LIBRARY intersect
	DOC "The CARVE polyhedron boolean library"
	)
MARK_AS_ADVANCED(K3D_CARVE_LIBRARY)

IF(K3D_CARVE_INCLUDE_DIR AND K3D_CARVE_LIBRARY)
	SET(K3D_CARVE_FOUND 1)
ENDIF()

