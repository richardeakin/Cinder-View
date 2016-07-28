cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( ViewTests )

# TODO: see if this can be a cache option and what happens there when it is set by the root
if( NOT CINDER_PATH )
	get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE )
endif()

get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE )

message( "[CinderView tests] CINDER_PATH: ${CINDER_PATH}" )
message( "[CinderView tests] APP_PATH: ${APP_PATH}" )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( APP_SOURCES
	${APP_PATH}/src/BasicViewTests.cpp
	${APP_PATH}/src/ControlsTest.cpp
	${APP_PATH}/src/CompositingTest.cpp
	${APP_PATH}/src/FilterTest.cpp
	${APP_PATH}/src/MultiTouchTest.cpp
	${APP_PATH}/src/ScrollTests.cpp
	${APP_PATH}/src/ViewTestsApp.cpp
)

#set( APP_INCLUDES
#	${PROJECT_SOURCE_DIR}/src
#	${APP_PATH}/src
#)

set( CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES COMPILE_DEFINITIONS )

ci_make_app(
	SOURCES         ${APP_SOURCES}
#	INCLUDES        ${APP_INCLUDES}
	CINDER_PATH     ${CINDER_PATH}
	BLOCKS          ../../../../Cinder-View
)
