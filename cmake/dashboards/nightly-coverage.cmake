# Setup default test variables ... 
SET(CTEST_CMAKE_COMMAND "\"${CMAKE_EXECUTABLE_NAME}\"")
SET(CTEST_COMMAND "\"${CTEST_EXECUTABLE_NAME}\" -D Nightly -A \"${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}\"")
SET(CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}/../..")
SET(CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

SET(CTEST_INITIAL_CACHE "

BUILD_TESTING:BOOL=ON
CMAKE_BUILD_TYPE:STRING=Debug
K3D_BUILD_MESH_MODULE:BOOL=OFF
K3D_BUILD_VIRTUAL_OFFSCREEN_MODULE:BOOL=OFF

")

#SET(CTEST_ENVIRONMENT
#  "CFLAGS=-g -O0 -Wall -W -fprofile-arcs -ftest-coverage"
#  "CXXFLAGS=-g -O0 -Wall -W -fprofile-arcs -ftest-coverage"
#  "LDFLAGS=-fprofile-arcs -ftest-coverage"
#)
SET(CTEST_ENVIRONMENT
  "CFLAGS=-g -O0 -fprofile-arcs -ftest-coverage"
  "CXXFLAGS=-g -O0 -fprofile-arcs -ftest-coverage"
  "LDFLAGS=-fprofile-arcs -ftest-coverage"
)

# Allow the caller to override test variables ... note that the caller MUST specify CTEST_BINARY_DIRECTORY at-a-minimum.
SET(VARIABLE)
FOREACH(ARGUMENT ${CTEST_SCRIPT_ARG})
	IF(VARIABLE)
		SET(${VARIABLE} ${ARGUMENT})
		SET(VARIABLE)
	ELSE(VARIABLE)
		SET(VARIABLE ${ARGUMENT})
	ENDIF(VARIABLE)
ENDFOREACH(ARGUMENT)

