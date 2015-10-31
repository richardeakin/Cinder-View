cmake_minimum_required( VERSION 2.8 )
project( ViewTests )

set( APP_TARGET "ViewTests" )
set( REPO_PATH "${PROJECT_SOURCE_DIR}/.." )
set( CINDER_PATH "${PROJECT_SOURCE_DIR}/../../cinder" )
set( CMAKE_VERBOSE_MAKEFILE ON )

include( "${REPO_PATH}/CMakeLists.txt" )
include( "${REPO_PATH}/cmake/modules/ci_make_app.cmake" )

set( APP_SOURCES
	src/BasicViewTests.cpp
	src/ControlsTest.cpp
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

set( VIEW_LIB "${REPO_PATH}/lib/${CMAKE_BUILD_TYPE}/libcinder-view.a" )
if( NOT EXISTS ${VIEW_LIB} )
	message( FATAL_ERROR "cannot find libcinder-view at path: ${VIEW_LIB}" )
endif()

target_link_libraries( ${APP_TARGET} ${VIEW_LIB} )
