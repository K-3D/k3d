PROJECT(doxygen)

INCLUDE(FindDoxygen)

CONFIGURE_FILE(${doxygen_SOURCE_DIR}/Doxyfile.in ${doxygen_BINARY_DIR}/Doxyfile)
CONFIGURE_FILE(${doxygen_SOURCE_DIR}/k3d.css ${doxygen_BINARY_DIR}/html/k3d.css COPYONLY)

ADD_CUSTOM_TARGET(docs-doxygen
	COMMAND ${DOXYGEN_EXECUTABLE} ${doxygen_BINARY_DIR}/Doxyfile
	)

