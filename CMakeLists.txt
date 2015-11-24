cmake_minimum_required( VERSION 3.1 )
project( cinder-view )

set( CMAKE_VERBOSE_MAKEFILE ON )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )

add_definitions( -Wfatal-errors )

if( NOT CINDER_PATH )
	set( CINDER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cinder )
endif()

set( VIEW_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src )

message( STATUS "cinder path: ${CINDER_PATH}" )

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
	${CINDER_PATH}/include
)

include_directories( ${VIEW_INCLUDE_USER} )
include_directories( ${VIEW_INCLUDE_SYSTEM} SYSTEM )

include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/view_sources.cmake )

add_library( cinder-view ${VIEW_SOURCES} )
