include( CMakeParseArguments )

function( ci_make_app )
	set( oneValueArgs APP_NAME CINDER_PATH )
	set( multiValueArgs SOURCES )

  	cmake_parse_arguments( ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	if( CINDER_BUILD_VERBOSE )
		message( STATUS "APP_NAME: ${ARG_APP_NAME}" )
		message( STATUS "SOURCES: ${ARG_SOURCES}" )
		message( STATUS "CINDER_PATH: ${ARG_CINDER_PATH}" )
	endif()

	if( ARG_UNPARSED_ARGUMENTS )
		message( WARNING "unhandled arguments: ${ARG_UNPARSED_ARGUMENTS}" )
	endif()

	if( NOT CMAKE_BUILD_TYPE )
		message( STATUS "Setting default CMAKE_BUILD_TYPE to Debug" )
		set( CMAKE_BUILD_TYPE Debug CACHE STRING 
		 	 "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel. "
		 	 FORCE
		)
	endif()

	# place the final app in a folder based on the build type.
	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE} )

	include_directories( "${ARG_CINDER_PATH}/include" )

	if( APPLE )
		# TODO: need to decide how to navigate the various places that libcinder can live
		# - probably best with the export + configure stuff
		set( CINDER_LIB cinder ) # the one built in the new cmake configuration

		# Find libcinder.a within cinder's lib folder for this build type
		# set( CINDER_LIB "${ARG_CINDER_PATH}/lib/${CMAKE_BUILD_TYPE}/libcinder.a" )

		# link in libcinder.a and redistributed dependencies
		list( APPEND CINDER_LINKED_LIBS
			${CINDER_LIB}
			${ARG_CINDER_PATH}/lib/macosx/libboost_filesystem.a
			${ARG_CINDER_PATH}/lib/macosx/libboost_system.a
			${ARG_CINDER_PATH}/lib/macosx/libz.a
		)

		# link in system frameworks
		find_library( COCOA_FRAMEWORK Cocoa )
		find_library( OPENGL_FRAMEWORK OpenGL )
		find_library( AVFOUNDATION_FRAMEWORK AVFoundation )
		find_library( AUDIOTOOLBOX_FRAMEWORK AudioToolbox )
		find_library( AUDIOUNIT_FRAMEWORK AudioUnit )
		find_library( COREAUDIO_FRAMEWORK CoreAudio )
		find_library( COREMEDIA_FRAMEWORK CoreMedia )
		find_library( COREVIDEO_FRAMEWORK CoreVideo )
		find_library( ACCELERATE_FRAMEWORK Accelerate )
		find_library( IOSURFACE_FRAMEWORK IOSurface )
		find_library( IOKIT_FRAMEWORK IOKit )

		list( APPEND CINDER_LINKED_LIBS
			${COCOA_FRAMEWORK}
			${OPENGL_FRAMEWORK}
			${AUDIOTOOLBOX_FRAMEWORK}
			${AVFOUNDATION_FRAMEWORK}
			${AUDIOUNIT_FRAMEWORK}
			${COREAUDIO_FRAMEWORK}
			${COREMEDIA_FRAMEWORK}
			${COREVIDEO_FRAMEWORK}
			${ACCELERATE_FRAMEWORK}
			${IOSURFACE_FRAMEWORK}
			${IOKIT_FRAMEWORK}
		)

		# TODO: move this out of this generic script:
		# set( VIEW_LIB "${PROJECT_SOURCE_DIR}/../lib/${CMAKE_BUILD_TYPE}/libcinder-view.a" )
		# if( NOT EXISTS ${VIEW_LIB} )
		# 	message( FATAL_ERROR "cannot find libcinder-view at path: ${VIEW_LIB}" )
		# endif()

		# list( APPEND CINDER_LINKED_LIBS ${VIEW_LIB} )

		# set icon
		set( ICON_NAME "CinderApp.icns" )
		set( ICON_PATH "${ARG_CINDER_PATH}/samples/data/${ICON_NAME}" )

	 	# copy .icns to bundle's resources folder
		set_source_files_properties( ${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources )

	else()
		message( FATAL_ERROR "unhandled platform: ${CMAKE_SYSTEM_NAME}" )
	endif()

#	if( NOT EXISTS ${CINDER_LIB} )
#		message( FATAL_ERROR "cannot find libcinder at path: ${CINDER_LIB}" )
#	endif()

	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" PARENT_SCOPE )
	# ??? should be set for CMakeCache.txt too? this is how to do it, though it overwrites everything else
	# set( CMAKE_CXX_FLAGS "-std=c++0x" CACHE STRING "compile flags" FORCE )

	add_executable( ${ARG_APP_NAME} MACOSX_BUNDLE WIN32 ${ARG_SOURCES} ${ICON_PATH} )

	target_link_libraries( ${ARG_APP_NAME} ${CINDER_LINKED_LIBS} )

	if( APPLE )
		# set bundle info.plist properties
	    set_target_properties( ${ARG_APP_NAME} PROPERTIES
	    	MACOSX_BUNDLE_BUNDLE_NAME ${ARG_APP_NAME}
	 		MACOSX_BUNDLE_ICON_FILE ${ICON_NAME}
	 	)
	endif()

endfunction()
