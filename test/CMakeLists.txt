cmake_minimum_required( VERSION 3.1 )
project( ViewTests )

set( APP_TARGET "ViewTests" )
set( REPO_PATH "${PROJECT_SOURCE_DIR}/.." )
set( CMAKE_VERBOSE_MAKEFILE ON )

message( STATUS "REPO_PATH: ${REPO_PATH}" )
message( STATUS "[test] CINDER_PATH_FULL: ${CINDER_PATH_FULL}" )

include( "${REPO_PATH}/cmake/ci_make_app.cmake" )

set( APP_SOURCES
	src/BasicViewTests.cpp
	src/ControlsTest.cpp
	src/CompositingTest.cpp
	src/ScrollTests.cpp
	src/ViewTestsApp.cpp
)

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${REPO_PATH}/src
)

ci_make_app(
	APP_NAME ${APP_TARGET}
	SOURCES ${APP_SOURCES}
	CINDER_PATH ${CINDER_PATH_FULL}
)

target_link_libraries( ${APP_TARGET} cinder-view )
