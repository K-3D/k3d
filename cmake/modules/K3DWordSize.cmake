# Detect the word-size for the current platform ...
MESSAGE(STATUS "checking the width of std::vector<>::size_type for this platform")

TRY_RUN(
	K3D_PLATFORM_SIZE_TYPE
	K3D_PLATFORM_SIZE_TYPE_COMPILE
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/configuration/size_type.cpp)

MESSAGE(STATUS "  std::vector<>::size_type is ${K3D_PLATFORM_SIZE_TYPE} bits")

IF(K3D_PLATFORM_SIZE_TYPE EQUAL 32)
	SET(K3D_UINT_T_32_BITS 1)
ELSEIF(K3D_PLATFORM_SIZE_TYPE EQUAL 64)
	SET(K3D_UINT_T_64_BITS 1)
ELSE(K3D_PLATFORM_SIZE_TYPE EQUAL 32)
	MESSAGE(SEND_ERROR "Error detecting platform word-size.")
ENDIF(K3D_PLATFORM_SIZE_TYPE EQUAL 32)

