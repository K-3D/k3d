# Generate self-extracting tarballs
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/package-binary-stgz.cpack.in ${CMAKE_CURRENT_BINARY_DIR}/package-binary-stgz.cpack)
ADD_CUSTOM_TARGET(package-binary-stgz
	${CMAKE_CPACK_COMMAND}
	--config ${CMAKE_CURRENT_BINARY_DIR}/package-binary-stgz.cpack
	)

# Generate tarballs
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/package-binary-tbz2.cpack.in ${CMAKE_CURRENT_BINARY_DIR}/package-binary-tbz2.cpack)
ADD_CUSTOM_TARGET(package-binary-tbz2
	${CMAKE_CPACK_COMMAND}
	--config ${CMAKE_CURRENT_BINARY_DIR}/package-binary-tbz2.cpack
	)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/package-binary-tgz.cpack.in ${CMAKE_CURRENT_BINARY_DIR}/package-binary-tgz.cpack)
ADD_CUSTOM_TARGET(package-binary-tgz
	${CMAKE_CPACK_COMMAND}
	--config ${CMAKE_CURRENT_BINARY_DIR}/package-binary-tgz.cpack
	)

