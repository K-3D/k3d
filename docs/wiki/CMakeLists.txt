PROJECT(wiki)

IF(NOT WIN32)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/make-wiki-docs-plugins.py ${CMAKE_CURRENT_BINARY_DIR}/make-wiki-docs-plugins.py)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/make-wiki-docs-primitives.py ${CMAKE_CURRENT_BINARY_DIR}/make-wiki-docs-primitives.py)

	ADD_CUSTOM_TARGET(docs-wiki
		COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/wikitext
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wikitext
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wikitext/articles
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wikitext/categories
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wikitext/templates
		COMMAND ${k3d_BINARY_DIR}/bin/k3d --log-level=debug --color --script-file=${CMAKE_CURRENT_BINARY_DIR}/make-wiki-docs-plugins.py --ui=nui
		COMMAND ${k3d_BINARY_DIR}/bin/k3d --log-level=debug --color --script-file=${CMAKE_CURRENT_BINARY_DIR}/make-wiki-docs-primitives.py --ui=nui
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		)
ENDIF(NOT WIN32)

