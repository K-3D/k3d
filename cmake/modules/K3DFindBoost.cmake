######################################################################
# Posix specific configuration

IF(UNIX AND NOT APPLE)
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
ENDIF(UNIX AND NOT APPLE)

######################################################################
# Apple specific configuration

IF(UNIX AND APPLE)
	FIND_PATH(K3D_BOOST_INCLUDE_DIR boost
		/opt/local/include
		DOC "Directory where the boost header files are located"
		)
	MARK_AS_ADVANCED(K3D_BOOST_INCLUDE_DIR)

	SET(K3D_BOOST_LIB_DIR /opt/local/lib CACHE PATH "Directory where the boost libraries are located")
	MARK_AS_ADVANCED(K3D_BOOST_LIB_DIR)

	SET(K3D_BOOST_PROGRAM_OPTIONS_LIB boost_program_options CACHE STRING "")
	MARK_AS_ADVANCED(K3D_BOOST_PROGRAM_OPTIONS_LIB)

	SET(K3D_BOOST_PYTHON_LIB boost_python CACHE STRING "")
	MARK_AS_ADVANCED(K3D_BOOST_PYTHON_LIB)

	SET(K3D_BOOST_REGEX_LIB boost_regex CACHE STRING "")
	MARK_AS_ADVANCED(K3D_BOOST_REGEX_LIB)
ENDIF(UNIX AND APPLE)

######################################################################
# Win32 specific configuration

IF(WIN32)
	FIND_PATH(K3D_BOOST_INCLUDE_DIR boost
		c:/boost/include/boost_1_34_1
		DOC "Directory where the boost header files are located"
		)
	MARK_AS_ADVANCED(K3D_BOOST_INCLUDE_DIR)

	SET(K3D_BOOST_LIB_DIR c:/boost/lib CACHE PATH "Directory where the boost libraries are located")
	MARK_AS_ADVANCED(K3D_BOOST_LIB_DIR)

        IF(MSVC)
                SET(K3D_BOOST_PROGRAM_OPTIONS_LIB optimized libboost_program_options-vc80-mt debug libboost_program_options-vc80-mt-gd CACHE STRING "")
        ELSE(MSVC)
                SET(K3D_BOOST_PROGRAM_OPTIONS_LIB boost_program_options-mgw34-1_34_1 CACHE STRING "")
        ENDIF(MSVC)
	MARK_AS_ADVANCED(K3D_BOOST_PROGRAM_OPTIONS_LIB)

        IF(MSVC)
                SET(K3D_BOOST_PYTHON_LIB optimized boost_python-vc80-mt debug boost_python-vc80-mt-gd CACHE STRING "")
        ELSE(MSVC)
                SET(K3D_BOOST_PYTHON_LIB boost_python-mgw34-1_34_1 CACHE STRING "")
        ENDIF(MSVC)
	MARK_AS_ADVANCED(K3D_BOOST_PYTHON_LIB)

        IF(MSVC)
                SET(K3D_BOOST_REGEX_LIB optimized boost_regex-vc80-mt debug boost_regex-vc80-mt-gd CACHE STRING "")
        ELSE(MSVC)
                SET(K3D_BOOST_REGEX_LIB boost_regex-mgw34-1_34_1 CACHE STRING "")
        ENDIF(MSVC)
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

