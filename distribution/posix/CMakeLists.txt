SET(K3D_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/K-3D ${K3D_VERSION}")
FILE(MAKE_DIRECTORY "${K3D_PACKAGE_DIRECTORY}")

# Generate self-extracting tarballs
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/package-binary-stgz.cpack.in ${CMAKE_CURRENT_BINARY_DIR}/package-binary-stgz.cpack)
ADD_CUSTOM_TARGET(package-binary-stgz
	WORKING_DIRECTORY "${K3D_PACKAGE_DIRECTORY}"
	COMMAND ${CMAKE_CPACK_COMMAND} --config ${CMAKE_CURRENT_BINARY_DIR}/package-binary-stgz.cpack
	)

# Generate tarballs
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/package-binary-tbz2.cpack.in ${CMAKE_CURRENT_BINARY_DIR}/package-binary-tbz2.cpack)
ADD_CUSTOM_TARGET(package-binary-tbz2
	WORKING_DIRECTORY "${K3D_PACKAGE_DIRECTORY}"
	COMMAND ${CMAKE_CPACK_COMMAND} --config ${CMAKE_CURRENT_BINARY_DIR}/package-binary-tbz2.cpack
	)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/package-binary-tgz.cpack.in ${CMAKE_CURRENT_BINARY_DIR}/package-binary-tgz.cpack)
ADD_CUSTOM_TARGET(package-binary-tgz
	WORKING_DIRECTORY "${K3D_PACKAGE_DIRECTORY}"
	COMMAND ${CMAKE_CPACK_COMMAND} --config ${CMAKE_CURRENT_BINARY_DIR}/package-binary-tgz.cpack
	)

# Note ... we intentionally don't provide upload capability for Posix packages, and leave distribution to the experts

