FIND_PATH(K3D_ZLIB_INCLUDE_DIR zlib.h
	/usr/include
	c:/gtk/include
	${K3D_GTK_DIR}/include
	"Directory where the zlib header files are located"
	)
MARK_AS_ADVANCED(K3D_ZLIB_INCLUDE_DIR)

SET(K3D_ZLIB_LIB z CACHE STRING "")
MARK_AS_ADVANCED(K3D_ZLIB_LIB)

SET(K3D_ZLIB_FOUND 1)

