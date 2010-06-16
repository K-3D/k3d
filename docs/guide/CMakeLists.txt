PROJECT(guide)

FIND_PROGRAM(ASCIIDOC_COMMAND asciidoc)
FIND_PROGRAM(A2X_COMMAND a2x)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/index.txt ${CMAKE_CURRENT_BINARY_DIR}/index.txt @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/index-docinfo.xml ${CMAKE_CURRENT_BINARY_DIR}/index-docinfo.xml @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/make-guide-plugins.py ${CMAKE_CURRENT_BINARY_DIR}/make-guide-plugins.py @ONLY)

SET(K3D_NUI_BINARY ${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins/k3d-nui.module)
ADD_CUSTOM_TARGET(docs-guide
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/plugins
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/plugins
	COMMAND ${k3d_BINARY_DIR}/bin/k3d --log-level=debug --color --plugins=${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins --share=${k3d_SOURCE_DIR}/share --script-file=${CMAKE_CURRENT_BINARY_DIR}/make-guide-plugins.py --ui=${K3D_NUI_BINARY}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

ADD_CUSTOM_TARGET(docs-guide-html
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/html
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/html
	COMMAND ${ASCIIDOC_COMMAND} --attribute docinfo --doctype book --backend docbook --out-file ${CMAKE_CURRENT_BINARY_DIR}/html/index.xml ${CMAKE_CURRENT_BINARY_DIR}/index.txt
	COMMAND ${A2X_COMMAND} -f chunked ${CMAKE_CURRENT_BINARY_DIR}/html/index.xml
	)

ADD_CUSTOM_TARGET(docs-guide-pdf
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/pdf
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/pdf
	COMMAND ${ASCIIDOC_COMMAND} --attribute docinfo --doctype book --backend docbook --out-file ${CMAKE_CURRENT_BINARY_DIR}/pdf/index.xml ${CMAKE_CURRENT_BINARY_DIR}/index.txt
	COMMAND ${A2X_COMMAND} -f pdf ${CMAKE_CURRENT_BINARY_DIR}/pdf/index.xml
	)
