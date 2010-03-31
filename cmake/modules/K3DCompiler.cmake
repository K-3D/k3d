IF(APPLE AND CMAKE_BUILD_TYPE STREQUAL "Release")
	EXECUTE_PROCESS(
		COMMAND ${CMAKE_CXX_COMPILER} --version
		OUTPUT_VARIABLE K3D_COMPILER_VERSION
	)

	IF(K3D_COMPILER_VERSION MATCHES "4.0.1")
		MESSAGE(SEND_ERROR "The Apple gcc 4.0.1 compiler produces release binaries that fail at runtime.  Try the gcc 4.2 compiler included in recent version of XCode instead.  See http://developer.k-3d.org/tracker/issue-cf2cdc4985a66db59f55052f03c3c620237b451f.html for details.")
	ENDIF()
ENDIF()

