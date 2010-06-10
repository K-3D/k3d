PROJECT(guide)

FIND_PROGRAM(ASCIIDOC_COMMAND asciidoc)
FIND_PROGRAM(A2X_COMMAND a2x)

ADD_CUSTOM_TARGET(docs-guide-html
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/html
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/html
	COMMAND ${ASCIIDOC_COMMAND} --attribute docinfo --doctype book --backend docbook --out-file ${CMAKE_CURRENT_BINARY_DIR}/html/index.xml ${CMAKE_CURRENT_SOURCE_DIR}/index.txt
	COMMAND ${A2X_COMMAND} -f chunked ${CMAKE_CURRENT_BINARY_DIR}/html/index.xml
	)

ADD_CUSTOM_TARGET(docs-guide-pdf
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/pdf
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/pdf
	COMMAND ${ASCIIDOC_COMMAND} --attribute docinfo --doctype book --backend docbook --out-file ${CMAKE_CURRENT_BINARY_DIR}/pdf/index.xml ${CMAKE_CURRENT_SOURCE_DIR}/index.txt
	COMMAND ${A2X_COMMAND} -f pdf ${CMAKE_CURRENT_BINARY_DIR}/pdf/index.xml
	)
