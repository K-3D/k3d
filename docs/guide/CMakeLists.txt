PROJECT(guide)

FIND_PROGRAM(ASCIIDOC_COMMAND asciidoc)
FIND_PROGRAM(A2X_COMMAND a2x)
FIND_PROGRAM(QHELPGENERATOR_COMMAND
	NAMES
		qhelpgenerator
		qhelpgenerator-mac
	)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/index.txt.in ${CMAKE_CURRENT_BINARY_DIR}/index.txt @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/index-docinfo.xml.in ${CMAKE_CURRENT_BINARY_DIR}/index-docinfo.xml @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/plugins.py.in ${CMAKE_CURRENT_BINARY_DIR}/plugins.py @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/guide.qhp.in ${CMAKE_CURRENT_BINARY_DIR}/guide.qhp @ONLY)

SET(K3D_NUI_BINARY ${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins/k3d-nui.module)
ADD_CUSTOM_COMMAND(
	DEPENDS
		${CMAKE_CURRENT_BINARY_DIR}/plugins.py
	WORKING_DIRECTORY
		${CMAKE_CURRENT_BINARY_DIR}
	COMMAND
		${k3d_BINARY_DIR}/bin/k3d --log-level=debug --color --plugins=${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins --share=${k3d_SOURCE_DIR}/share --script-file=${CMAKE_CURRENT_BINARY_DIR}/plugins.py --ui=${K3D_NUI_BINARY}
	OUTPUT
		${CMAKE_CURRENT_BINARY_DIR}/plugins.txt
	)

ADD_CUSTOM_TARGET(docs-guide-html
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/plugins.txt
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/html
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/html
	COMMAND ${ASCIIDOC_COMMAND} --attribute docinfo --doctype book --backend docbook --out-file ${CMAKE_CURRENT_BINARY_DIR}/html/index.xml ${CMAKE_CURRENT_BINARY_DIR}/index.txt
	COMMAND ${A2X_COMMAND} -v -f chunked ${CMAKE_CURRENT_BINARY_DIR}/html/index.xml
	)

ADD_CUSTOM_TARGET(docs-guide-pdf
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/plugins.txt
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/pdf
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/pdf
	COMMAND ${ASCIIDOC_COMMAND} --attribute docinfo --doctype book --backend docbook --out-file ${CMAKE_CURRENT_BINARY_DIR}/pdf/index.xml ${CMAKE_CURRENT_BINARY_DIR}/index.txt
	COMMAND ${A2X_COMMAND} -v --destination-dir ${CMAKE_CURRENT_BINARY_DIR}/pdf -f pdf ${CMAKE_CURRENT_BINARY_DIR}/pdf/index.xml
	)

ADD_CUSTOM_TARGET(docs-guide-qhc
	COMMAND ${QHELPGENERATOR_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/guide.qhp -o ${CMAKE_CURRENT_BINARY_DIR}/guide.qch
	)
