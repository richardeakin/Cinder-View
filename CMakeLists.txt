cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( Cinder-View )

# Cache options
if( NOT CINDER_PATH )
	message( "no CINDER_PATH defined" )
	set( CINDER_PATH "../.." CACHE STRING "Path to Cinder directory" )
else()
	message( "CINDER_PATH already set to: ${CINDER_PATH}" )
endif()

option( CINDER_VIEW_TEST_ENABLE "ViewTests" ON )

get_filename_component( CINDER_PATH "${CINDER_PATH}" ABSOLUTE )

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE Debug CACHE STRING
		 "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel. "
		 FORCE
	)
else()
	message( STATUS "CMAKE_BUILD_TYPE set to ${CMAKE_BUILD_TYPE}" )
endif()

#set( VIEW_LIB_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE} )

message( "[cinder-view root] VIEW_LIB_DIRECTORY: ${VIEW_LIB_DIRECTORY}" )

#add_definitions( -Wfatal-errors )

# TODO NEXT: how do we still place the output in a lib folder? Should the CMAKE_ARCHIVE_OUTPUT_DIRECTORY setting be in the confgi.cmake?
find_package( Cinder-View REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/proj/cmake NO_DEFAULT_PATH )

#export( TARGETS cinder-view FILE cinder-view-exports.cmake )

if( CINDER_VIEW_TEST_ENABLE )
	add_subdirectory( test/proj/cmake )
endif()
