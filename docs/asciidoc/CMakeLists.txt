PROJECT(asciidoc)

FIND_PROGRAM(ASCIIDOC_COMMAND asciidoc)
FIND_PROGRAM(DBLATEX_COMMAND dblatex)

#CONFIGURE_FILE(${doxygen_SOURCE_DIR}/Doxyfile.in ${doxygen_BINARY_DIR}/Doxyfile)

ADD_CUSTOM_TARGET(docs-asciidoc-html
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/html
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/html
	COMMAND ${ASCIIDOC_COMMAND} --doctype book --backend xhtml11 --out-file ${CMAKE_CURRENT_BINARY_DIR}/html/index.html ${CMAKE_CURRENT_SOURCE_DIR}/index.txt
	)

ADD_CUSTOM_TARGET(docs-asciidoc-pdf
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/pdf
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/pdf
	COMMAND ${ASCIIDOC_COMMAND} --attribute docinfo --doctype book --backend docbook --out-file ${CMAKE_CURRENT_BINARY_DIR}/pdf/index.xml ${CMAKE_CURRENT_SOURCE_DIR}/index.txt
	COMMAND ${DBLATEX_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/pdf/index.xml
	)
