# Test to ensure that new code never uses k3d::typed_array<k3d::uint_t>

FUNCTION(TEST_FILES GLOB_EXPRESSION)
	FILE(GLOB_RECURSE SOURCE_FILES ${GLOB_EXPRESSION})

	FOREACH(SOURCE_FILE ${SOURCE_FILES})
		FILE(READ ${SOURCE_FILE} SOURCE)
		IF(SOURCE MATCHES "typed_array *< *(k3d::)?uint_t *>")
			MESSAGE(SEND_ERROR "${SOURCE_FILE} uses k3d::typed_array<k3d::uint_t> ... use k3d::uint_t_array instead.")
		ENDIF()
	ENDFOREACH(SOURCE_FILE)

ENDFUNCTION(TEST_FILES)

TEST_FILES(*.h)
TEST_FILES(*.cpp)

