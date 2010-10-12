PROJECT(guide)

# Note - we build all of the guide-related targets from this one listfile because
# it makes it much easier to handle custom command dependencies.

SET(CONTENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/content)
SET(CONTENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/content)
FILE(MAKE_DIRECTORY ${CONTENT_BINARY_DIR})

SET(DOCBOOK_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/docbook)
FILE(MAKE_DIRECTORY ${DOCBOOK_BINARY_DIR})

SET(CHUNKED_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/chunked)
FILE(MAKE_DIRECTORY ${CHUNKED_BINARY_DIR})

SET(HTML_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/html)
FILE(MAKE_DIRECTORY ${HTML_BINARY_DIR})

SET(PDF_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/pdf)
FILE(MAKE_DIRECTORY ${PDF_BINARY_DIR})

SET(QT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qt)
SET(QT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/qt)
FILE(MAKE_DIRECTORY ${CONTENT_BINARY_DIR})

CONFIGURE_FILE(${CONTENT_SOURCE_DIR}/guide-docinfo.xml.in ${CONTENT_BINARY_DIR}/guide-docinfo.xml @ONLY)
CONFIGURE_FILE(${CONTENT_SOURCE_DIR}/guide.txt.in ${CONTENT_BINARY_DIR}/guide.txt @ONLY)
CONFIGURE_FILE(${CONTENT_SOURCE_DIR}/plugins.py.in ${CONTENT_BINARY_DIR}/plugins.py @ONLY)

CONFIGURE_FILE(${QT_SOURCE_DIR}/about.txt.in ${QT_BINARY_DIR}/about.txt @ONLY)
CONFIGURE_FILE(${QT_SOURCE_DIR}/guide.qhcp.in ${QT_BINARY_DIR}/guide.qhcp @ONLY)
CONFIGURE_FILE(${QT_SOURCE_DIR}/guide.qhp.in ${QT_BINARY_DIR}/guide.qhp @ONLY)
CONFIGURE_FILE(${share_SOURCE_DIR}/k3d/icons/k3d.png ${QT_BINARY_DIR}/k3d.png COPYONLY)

FILE(GLOB GUIDE_CONTENT ${CONTENT_SOURCE_DIR}/*.txt)
LIST(APPEND GUIDE_CONTENT ${CONTENT_BINARY_DIR}/guide.txt)
LIST(APPEND GUIDE_CONTENT ${CONTENT_BINARY_DIR}/guide-docinfo.xml)
LIST(APPEND GUIDE_CONTENT ${CONTENT_BINARY_DIR}/plugins.txt)
LIST(SORT GUIDE_CONTENT)

# Setup generation of the reference documentation ...
GET_TARGET_PROPERTY(K3D_COMMAND k3d LOCATION)

ADD_CUSTOM_COMMAND(
	DEPENDS k3d k3d-python ${CONTENT_BINARY_DIR}/plugins.py
	WORKING_DIRECTORY ${CONTENT_BINARY_DIR}
	COMMAND ${K3D_COMMAND} --log-level=debug --color --script-file=${CONTENT_BINARY_DIR}/plugins.py --ui=nui
	OUTPUT ${CONTENT_BINARY_DIR}/plugins.txt
	)

ADD_CUSTOM_COMMAND(
	DEPENDS ${GUIDE_CONTENT}
	COMMAND ${ASCIIDOC_COMMAND} --attribute docinfo --doctype book --backend docbook --out-file ${DOCBOOK_BINARY_DIR}/guide.xml ${CONTENT_BINARY_DIR}/guide.txt
	OUTPUT ${DOCBOOK_BINARY_DIR}/guide.xml
	)

ADD_CUSTOM_TARGET(docs-guide-docbook
	DEPENDS ${DOCBOOK_BINARY_DIR}/guide.xml
	)

ADD_CUSTOM_COMMAND(
	DEPENDS ${DOCBOOK_BINARY_DIR}/guide.xml
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${DOCBOOK_BINARY_DIR} ${CHUNKED_BINARY_DIR}
	COMMAND ${A2X_COMMAND} -v --format chunked ${CHUNKED_BINARY_DIR}/guide.xml
	OUTPUT ${CHUNKED_BINARY_DIR}/guide.chunked/index.html
	)

# Setup optional generation of a standalone HTML version of the guide ...
ADD_CUSTOM_TARGET(docs-guide-html
	DEPENDS ${CHUNKED_BINARY_DIR}/guide.chunked/index.html
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CHUNKED_BINARY_DIR}/guide.chunked ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/html
	)

# Setup optional generation of a standalone PDF version of the guide ...
ADD_CUSTOM_TARGET(docs-guide-pdf
	DEPENDS ${DOCBOOK_BINARY_DIR}/guide.xml
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${DOCBOOK_BINARY_DIR} ${PDF_BINARY_DIR}
	COMMAND ${A2X_COMMAND} -v --format pdf ${PDF_BINARY_DIR}/guide.xml
	)

# Setup generation of an HTML version of the guide for use with the NGUI ...
IF(K3D_BUILD_NGUI_MODULE)
	ADD_CUSTOM_COMMAND(
		DEPENDS ${CHUNKED_BINARY_DIR}/guide.chunked/index.html
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CHUNKED_BINARY_DIR}/guide.chunked ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/html
		OUTPUT ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/html/index.html
		)

	ADD_CUSTOM_TARGET(docs-guide-ngui ALL
		DEPENDS ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/html/index.html
		)

	INSTALL(DIRECTORY ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/html DESTINATION share/k3d/guide)
ENDIF()

# Setup generation of a Qt Assistant version of the guide for use with the QTUI ...
IF(K3D_BUILD_QTUI_MODULE)
	FIND_PROGRAM(QHELPGENERATOR_COMMAND NAMES qhelpgenerator qhelpgenerator-mac)
	FIND_PROGRAM(QCOLLECTIONGENERATOR_COMMAND NAMES qcollectiongenerator qcollectiongenerator-mac)

	ADD_CUSTOM_COMMAND(
		DEPENDS
			${CHUNKED_BINARY_DIR}/guide.chunked/index.html
			${QT_BINARY_DIR}/guide.qhp
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CHUNKED_BINARY_DIR}/guide.chunked ${QT_BINARY_DIR}
		COMMAND ${QHELPGENERATOR_COMMAND} ${QT_BINARY_DIR}/guide.qhp -o ${QT_BINARY_DIR}/guide.qch
		OUTPUT ${QT_BINARY_DIR}/guide.qch
		)

	ADD_CUSTOM_COMMAND(
		DEPENDS
			${QT_BINARY_DIR}/about.txt
			${QT_BINARY_DIR}/guide.qhcp
			${QT_BINARY_DIR}/guide.qch
			${QT_BINARY_DIR}/k3d.png
		COMMAND ${QCOLLECTIONGENERATOR_COMMAND} ${QT_BINARY_DIR}/guide.qhcp -o ${QT_BINARY_DIR}/guide.qhc
		OUTPUT ${QT_BINARY_DIR}/guide.qhc
		)

	ADD_CUSTOM_COMMAND(
		DEPENDS ${QT_BINARY_DIR}/guide.qch
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/guide.qch ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/guide.qch
		OUTPUT ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/guide.qch
		)

	ADD_CUSTOM_COMMAND(
		DEPENDS ${QT_BINARY_DIR}/guide.qhc
		COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/guide.qhc ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/guide.qhc
		OUTPUT ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/guide.qhc
		)

	ADD_CUSTOM_TARGET(docs-guide-qtui ALL
		DEPENDS ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/guide.qch
		DEPENDS ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/guide.qhc
		)

	INSTALL(FILES ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/guide.qch DESTINATION share/k3d/guide)
	INSTALL(FILES ${K3D_SHARE_OUTPUT_DIRECTORY}/k3d/guide/guide.qhc DESTINATION share/k3d/guide)
ENDIF()
