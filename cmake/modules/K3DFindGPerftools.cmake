SET(K3D_GPERFTOOLS_FOUND 0)

FIND_PATH(K3D_GPERFTOOLS_INCLUDE_DIR gperftools
	/usr/include
	/usr/local/include
	DOC "Directory where the gperftools directory, containing the perftools headers, is located"
	)
MARK_AS_ADVANCED(K3D_GPERFTOOLS_INCLUDE_DIR)

FIND_LIBRARY(K3D_PROFILER_LIBRARY profiler
	DOC "The gperftools profiler library"
	)
MARK_AS_ADVANCED(K3D_PROFILER_LIBRARY)

IF(K3D_GPERFTOOLS_INCLUDE_DIR AND K3D_PROFILER_LIBRARY)
	SET(K3D_GPERFTOOLS_FOUND 1)
ENDIF()
