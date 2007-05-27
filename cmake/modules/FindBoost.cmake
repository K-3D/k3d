######################################################################
# Posix specific configuration

IF(UNIX)
	# Configure boost ...
	FIND_PATH(K3D_BOOST_INCLUDE_DIR boost
		/usr/include
		DOC "Directory where the boost header files are located"
		)
	MARK_AS_ADVANCED(K3D_BOOST_INCLUDE_DIR)

	SET(K3D_BOOST_LIB_DIR /usr/lib CACHE PATH "Directory where the boost libraries are located")
	MARK_AS_ADVANCED(K3D_BOOST_LIB_DIR)

	SET(K3D_BOOST_PROGRAM_OPTIONS_LIB boost_program_options CACHE STRING "")
	MARK_AS_ADVANCED(K3D_BOOST_PROGRAM_OPTIONS_LIB)

	SET(K3D_BOOST_PYTHON_LIB boost_python CACHE STRING "")
	MARK_AS_ADVANCED(K3D_BOOST_PYTHON_LIB)

	SET(K3D_BOOST_REGEX_LIB boost_regex CACHE STRING "")
	MARK_AS_ADVANCED(K3D_BOOST_REGEX_LIB)

ENDIF(UNIX)

######################################################################
# Win32 specific configuration

IF(WIN32)
	# Configure boost ...
	FIND_PATH(K3D_BOOST_INCLUDE_DIR boost
		c:/boost/include/boost-1_33_1
		DOC "Directory where the boost header files are located"
		)
	MARK_AS_ADVANCED(K3D_BOOST_INCLUDE_DIR)

	SET(K3D_BOOST_LIB_DIR c:/boost/lib CACHE PATH "Directory where the boost libraries are located")
	MARK_AS_ADVANCED(K3D_BOOST_LIB_DIR)

	SET(K3D_BOOST_PROGRAM_OPTIONS_LIB libboost_program_options-mgw CACHE STRING "")
	MARK_AS_ADVANCED(K3D_BOOST_PROGRAM_OPTIONS_LIB)

	SET(K3D_BOOST_PYTHON_LIB boost_python-mgw CACHE STRING "")
	MARK_AS_ADVANCED(K3D_BOOST_PYTHON_LIB)

	SET(K3D_BOOST_REGEX_LIB boost_regex-mgw CACHE STRING "")
	MARK_AS_ADVANCED(K3D_BOOST_REGEX_LIB)

ENDIF(WIN32)

# Following are the consolidated variables that should be used for builds
SET(K3D_BOOST_INCLUDE_DIRS
	${K3D_BOOST_INCLUDE_DIR}
	)

SET(K3D_BOOST_LIB_DIRS
	${K3D_BOOST_LIB_DIR}
	)

SET(K3D_BOOST_PROGRAM_OPTIONS_LIBS
	${K3D_BOOST_PROGRAM_OPTIONS_LIB}
	)

SET(K3D_BOOST_PYTHON_LIBS
	${K3D_BOOST_PYTHON_LIB}
	)

SET(K3D_BOOST_REGEX_LIBS
	${K3D_BOOST_REGEX_LIB}
	)

SET(K3D_BOOST_FOUND 1)

