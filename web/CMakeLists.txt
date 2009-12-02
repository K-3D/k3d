PROJECT(web)

SET(web_OUTPUT_DIR "${web_BINARY_DIR}/html")

SET(developer_OUTPUT_DIR "${web_OUTPUT_DIR}/developer")
SET(planet_OUTPUT_DIR "${web_OUTPUT_DIR}/planet")
SET(tracker_OUTPUT_DIR "${web_OUTPUT_DIR}/www/tracker")
SET(www_OUTPUT_DIR "${web_OUTPUT_DIR}/www")

ADD_CUSTOM_TARGET(developer-html
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${web_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${developer_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.css" "${developer_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.png" "${developer_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/developer/developer.css" "${developer_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/developer/logo.png" "${developer_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/developer" mako-render "index.xhtml" > "${developer_OUTPUT_DIR}/index.xhtml"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/developer" mako-render "source.xhtml" > "${developer_OUTPUT_DIR}/source.xhtml"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/developer" mako-render "documentation.xhtml" > "${developer_OUTPUT_DIR}/documentation.xhtml"
	)

ADD_CUSTOM_TARGET(planet-html
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${web_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.css" "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.png" "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/logo.png" "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/planet/feed-icon-10x10.png" "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/planet/planet.css" "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/planet/planet" --templates "${CMAKE_CURRENT_SOURCE_DIR}/planet" --output "${planet_OUTPUT_DIR}"
	)

ADD_CUSTOM_TARGET(tracker-html
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${web_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${tracker_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/www/tracker/tracker.css" "${tracker_OUTPUT_DIR}"
	COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/www/tracker/tracker" --project "${ditz_SOURCE_DIR}" --templates "${CMAKE_CURRENT_SOURCE_DIR}/www/tracker" --output "${tracker_OUTPUT_DIR}"
	)

ADD_CUSTOM_TARGET(web-html
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${web_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.css" "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.png" "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/logo.png" "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/www/donations.xhtml" "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/www/downloads.xhtml" "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/www/index.xhtml" "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/www/maintenance.xhtml" "${www_OUTPUT_DIR}"
	)

