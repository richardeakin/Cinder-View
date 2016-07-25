cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
project( cinder-view )

# Cache options
set( CINDER_VIEW_CINDER_PATH "../../cinder" CACHE STRING "Path to Cinder directory" )
option( CINDER_VIEW_TEST_ENABLE "ViewTests" ON )


set( CMAKE_VERBOSE_MAKEFILE ON )

get_filename_component( CINDER_PATH_FULL "${CMAKE_CURRENT_LIST_DIR}/${CINDER_VIEW_CINDER_PATH}" ABSOLUTE )
get_filename_component( VIEW_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src" ABSOLUTE )

message( STATUS "[cinder-view] cinder path: ${CINDER_PATH_FULL}" )

if( NOT CMAKE_BUILD_TYPE )
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
	${CINDER_PATH_FULL}/include
)

set( VIEW_INCLUDE_SYSTEM
	${CINDER_PATH_FULL}/include
)

include_directories( ${VIEW_INCLUDE_USER} )
include_directories( ${VIEW_INCLUDE_SYSTEM} SYSTEM )

add_definitions( -Wfatal-errors )

include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/libcinder-view.cmake )

#export( TARGETS cinder-view FILE cinder-view-exports.cmake )

if( CINDER_VIEW_TEST_ENABLE )
	add_subdirectory( test )
endif()
