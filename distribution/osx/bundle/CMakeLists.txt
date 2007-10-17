# Setup creation of an application bundle
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/create_bundle.sh.in ${CMAKE_CURRENT_BINARY_DIR}/create_bundle.sh @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist @ONLY)

ADD_CUSTOM_TARGET(package-bundle
	COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/create_bundle.sh
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

