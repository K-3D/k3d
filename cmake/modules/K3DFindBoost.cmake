SET(K3D_BOOST_FOUND 0)

######################################################################
# Posix specific configuration

IF(UNIX)

	SET(Boost_ADDITIONAL_VERSIONS 1.37 1.38)
	FIND_PACKAGE(Boost 1.34.1 COMPONENTS program_options python regex)

	SET(K3D_BOOST_FOUND ${Boost_FOUND})

	SET(K3D_BOOST_INCLUDE_DIRS
		${Boost_INCLUDE_DIRS}
		)
	SET(K3D_BOOST_PROGRAM_OPTIONS_LIBS
		${Boost_PROGRAM_OPTIONS_LIBRARY}
		)
	SET(K3D_BOOST_PYTHON_LIBS
		${Boost_PYTHON_LIBRARY}
		)
	SET(K3D_BOOST_REGEX_LIBS
		${Boost_REGEX_LIBRARY}
		)

ENDIF(UNIX)

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

ENDIF(WIN32)

