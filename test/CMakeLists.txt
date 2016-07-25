cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
project( ViewTests )

set( APP_TARGET "ViewTests" )
set( REPO_PATH "${PROJECT_SOURCE_DIR}/.." )
set( CMAKE_VERBOSE_MAKEFILE ON )

message( STATUS "[CinderView tests] REPO_PATH: ${REPO_PATH}" )
message( STATUS "[CinderView tests] CINDER_PATH_FULL: ${CINDER_PATH_FULL}" )

include( "${CINDER_PATH_FULL}/proj/cmake/modules/cinderMakeApp.cmake" )

set( APP_SOURCES
	src/BasicViewTests.cpp
	src/ControlsTest.cpp
	src/CompositingTest.cpp
	src/MultiTouchTest.cpp
	src/ScrollTests.cpp
	src/ViewTestsApp.cpp
)

set( APP_INCLUDES
	${PROJECT_SOURCE_DIR}/src
	${REPO_PATH}/src
)

ci_make_app(
	APP_NAME        ${APP_TARGET}
	SOURCES         ${APP_SOURCES}
	INCLUDES        ${APP_INCLUDES}
	CINDER_PATH     ${CINDER_PATH_FULL}
)

add_dependencies( ${APP_TARGET} cinder-view )
target_link_libraries( ${APP_TARGET} cinder-view )
