FIND_PATH(K3D_CGAL_INCLUDE_DIR CGAL
	/usr/include
	/usr/local/include 
	DOC "Directory where the CGAL header directory is located"
	)
MARK_AS_ADVANCED(K3D_CGAL_INCLUDE_DIR)

SET(K3D_CGAL_FOUND 1)

