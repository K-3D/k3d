# Generate NSIS installer
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/package-binary-nsis.cpack.in ${CMAKE_CURRENT_BINARY_DIR}/package-binary-nsis.cpack)
ADD_CUSTOM_TARGET(package-binary-nsis
	${CMAKE_CPACK_COMMAND}
	--config ${CMAKE_CURRENT_BINARY_DIR}/package-binary-nsis.cpack
	)

