if( NOT TARGET Cinder-View )

	include( ${CINDER_PATH}/proj/cmake/utilities.cmake )

	ci_log_v( "cinder path: ${CINDER_PATH}" )

	get_filename_component( VIEW_SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../src" ABSOLUTE )
	get_filename_component( VIEW_LIB_PATH "${CMAKE_CURRENT_LIST_DIR}/../../lib/${CMAKE_BUILD_TYPE}" ABSOLUTE )

	ci_log_v( "VIEW_LIB_PATH: ${VIEW_LIB_PATH}" )

	list( APPEND VIEW_SOURCES
		${VIEW_SOURCE_PATH}/ui/Control.cpp
		${VIEW_SOURCE_PATH}/ui/Filter.cpp
		${VIEW_SOURCE_PATH}/ui/GestureTracker.cpp
		${VIEW_SOURCE_PATH}/ui/Graph.cpp
		${VIEW_SOURCE_PATH}/ui/Image.cpp
		${VIEW_SOURCE_PATH}/ui/ImageView.cpp
		${VIEW_SOURCE_PATH}/ui/Interface3d.cpp
		${VIEW_SOURCE_PATH}/ui/Label.cpp
		${VIEW_SOURCE_PATH}/ui/Layer.cpp
		${VIEW_SOURCE_PATH}/ui/Layout.cpp
		${VIEW_SOURCE_PATH}/ui/Renderer.cpp
		${VIEW_SOURCE_PATH}/ui/ScrollView.cpp
		${VIEW_SOURCE_PATH}/ui/Suite.cpp
		${VIEW_SOURCE_PATH}/ui/TextManager.cpp
		${VIEW_SOURCE_PATH}/ui/TextField.cpp
		${VIEW_SOURCE_PATH}/ui/View.cpp
	)

	# cppformat
	list( APPEND VIEW_SOURCES
		${VIEW_SOURCE_PATH}/fmt/format.cc
	)

	add_library( Cinder-View ${VIEW_SOURCES} )

	set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VIEW_LIB_DIRECTORY} )

	set( VIEW_INCLUDE_USER
		${VIEW_SOURCE_PATH}
		${CINDER_PATH}/include
	)

	set( VIEW_INCLUDE_SYSTEM
		${CINDER_PATH}/include
	)

	target_include_directories( Cinder-View PUBLIC ${VIEW_INCLUDE_USER} )
	target_include_directories( Cinder-View SYSTEM BEFORE PUBLIC ${VIEW_INCLUDE_SYSTEM} )

	target_compile_options( Cinder-View PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++14> )
endif()
