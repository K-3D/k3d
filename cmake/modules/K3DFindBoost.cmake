SET(K3D_BOOST_FOUND 0)

SET(Boost_ADDITIONAL_VERSIONS 1.37 1.38)
IF(WIN32)
	SET(BOOST_ROOT "C:/Program Files/Boost")
	SET(Boost_COMPILER -gcc43)
ENDIF(WIN32)

#Make sure we only search for the system libary after boost version 1.35, courtesy of Geir Erikstad
find_package(Boost)
set(local_boost_version "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}")
if (${local_boost_version} VERSION_LESS "1.35")
   FIND_PACKAGE(Boost 1.34 COMPONENTS filesystem program_options python regex thread)
else()
   FIND_PACKAGE(Boost 1.34 COMPONENTS filesystem program_options python regex thread system)
endif()

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
# Needed for collada_io
SET(K3D_BOOST_FILESYSTEM_LIBS
	${Boost_FILESYSTEM_LIBRARY}
	)
SET(K3D_BOOST_SYSTEM_LIBS
	${Boost_SYSTEM_LIBRARY}
	)
# Needed for CGAL
SET(K3D_BOOST_THREAD_LIBS
	${Boost_THREAD_LIBRARY}
	)
