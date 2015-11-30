cmake_minimum_required( VERSION 3.1 )
project( ViewTests )

set( APP_TARGET "ViewTests" )
set( REPO_PATH "${PROJECT_SOURCE_DIR}/.." )
set( CINDER_PATH "${PROJECT_SOURCE_DIR}/../../cinder" )
set( CMAKE_VERBOSE_MAKEFILE ON )

message( STATUS "REPO_PATH: ${REPO_PATH}" )

get_property( inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES )
message( STATUS "[test] include dirs: ${inc_dirs}" )

include( "${REPO_PATH}/cmake/ci_make_app.cmake" )

set( APP_SOURCES
	src/BasicViewTests.cpp
	src/ControlsTest.cpp
	src/LayerTest.cpp
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
	CINDER_PATH ${CINDER_PATH}
)

target_link_libraries( ${APP_TARGET} cinder-view )
