FIND_PATH(K3D_FREETYPE2_CONFIG_INCLUDE_DIR ft2build.h
	/usr/include
	c:/gtk/include
	DOC "Directory where the freetype2 config files are located"
	)
MARK_AS_ADVANCED(K3D_FREETYPE2_CONFIG_INCLUDE_DIR)

FIND_PATH(K3D_FREETYPE2_INCLUDE_DIR freetype
	/usr/include/freetype2
	c:/gtk/include/freetype2
	DOC "Directory where the freetype2 header files are located"
	)
MARK_AS_ADVANCED(K3D_FREETYPE2_INCLUDE_DIR)

SET(K3D_FREETYPE2_LIB freetype CACHE STRING "")
MARK_AS_ADVANCED(K3D_FREETYPE2_LIB)

SET(K3D_FREETYPE2_FOUND 1)

