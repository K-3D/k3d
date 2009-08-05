PROJECT(doxygen)

INCLUDE(FindDoxygen)

CONFIGURE_FILE(${doxygen_SOURCE_DIR}/Doxyfile.in ${doxygen_BINARY_DIR}/Doxyfile)

ADD_CUSTOM_TARGET(docs-doxygen
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${doxygen_BINARY_DIR}/html
	COMMAND ${DOXYGEN_EXECUTABLE} ${doxygen_BINARY_DIR}/Doxyfile
	COMMAND ${CMAKE_COMMAND} -E copy ${doxygen_SOURCE_DIR}/k3d.css ${doxygen_BINARY_DIR}/html/k3d.css
	)

