cmake_minimum_required( VERSION 3.1 )
project( cinder-view )

# Cache options
set( CINDER_PATH "../cinder" CACHE STRING "Path to Cinder directory" )
option( CINDER_VIEW_TEST_ENABLE "ViewTests" OFF )


set( CMAKE_VERBOSE_MAKEFILE ON )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )

add_definitions( -Wfatal-errors )

get_filename_component( CINDER_PATH_FULL "${CMAKE_CURRENT_SOURCE_DIR}/../cinder" ABSOLUTE )
get_filename_component( VIEW_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src" ABSOLUTE )

message( STATUS "cinder path: ${CINDER_PATH_FULL}" )

if( NOT CMAKE_BUILD_TYPE )
	message( STATUS "CMAKE_BUILD_TYPE not specified, defaulting to Debug" )
	set( CMAKE_BUILD_TYPE Debug CACHE STRING 
		 "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel. "
		 FORCE
	)
else()
	message( STATUS "CMAKE_BUILD_TYPE set to ${CMAKE_BUILD_TYPE}" )
endif()

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE} )

set( VIEW_INCLUDE_USER
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CINDER_PATH}/include
)

set( VIEW_INCLUDE_SYSTEM
	${CINDER_PATH_FULL}/include
)

include_directories( ${VIEW_INCLUDE_USER} )
include_directories( ${VIEW_INCLUDE_SYSTEM} SYSTEM )

include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/view_sources.cmake )

add_library( cinder-view ${VIEW_SOURCES} )

#export( TARGETS cinder-view FILE cinder-view-exports.cmake )

# TODO: can't build with a target in CLion
#if( CINDER_VIEW_TEST_ENABLE )
	add_subdirectory( test )
#endif()
