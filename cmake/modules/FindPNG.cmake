FIND_PATH(K3D_PNG_INCLUDE_DIR png.h
	/usr/include/libpng12
	c:/gtk/include/libpng12
	${K3D_GTK_DIR}/include/libpng12
	DOC "Directory where the libpng header files are located"
	)
MARK_AS_ADVANCED(K3D_PNG_INCLUDE_DIR)

SET(K3D_PNG_LIB png CACHE STRING "")
MARK_AS_ADVANCED(K3D_PNG_LIB)

SET(K3D_PNG_INCLUDE_DIRS
	${K3D_PNG_INCLUDE_DIR}
	${K3D_ZLIB_INCLUDE_DIR}
	)

SET(K3D_PNG_LIBS
	${K3D_PNG_LIB}
	)

SET(K3D_PNG_FOUND 1)

