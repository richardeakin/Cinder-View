cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )

project( ViewTests )

if( NOT CINDER_PATH )
	set( CINDER_PATH "../../../../.." CACHE STRING "Path to Cinder directory" )
endif()

get_filename_component( CINDER_PATH "${CINDER_PATH}" ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE )

message( "[CinderView tests] CINDER_PATH: ${CINDER_PATH}" )
message( "[CinderView tests] APP_PATH: ${APP_PATH}" )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( APP_SOURCES
	${APP_PATH}/src/BasicViewTests.cpp
	${APP_PATH}/src/ControlsTest.cpp
	${APP_PATH}/src/CompositingTest.cpp
	${APP_PATH}/src/FilterTest.cpp
	${APP_PATH}/src/LayoutTests.cpp
	${APP_PATH}/src/MultiTouchTest.cpp
	${APP_PATH}/src/ScrollTests.cpp
	${APP_PATH}/src/ViewTestsApp.cpp
)

#set( APP_INCLUDES
#	${PROJECT_SOURCE_DIR}/src
#	${APP_PATH}/src
#)

#set( CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES COMPILE_DEFINITIONS )

ci_make_app(
	SOURCES         ${APP_SOURCES}
#	INCLUDES        ${APP_INCLUDES}
	CINDER_PATH     ${CINDER_PATH}
	BLOCKS          ../../../../Cinder-View
)
