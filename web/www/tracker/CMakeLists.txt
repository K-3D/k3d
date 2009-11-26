PROJECT(tracker)

ADD_CUSTOM_TARGET(tracker-html
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/html"
	COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/tracker" --project "${ditz_SOURCE_DIR}" --templates "${CMAKE_CURRENT_SOURCE_DIR}" --output "${CMAKE_CURRENT_BINARY_DIR}/html"
	)
