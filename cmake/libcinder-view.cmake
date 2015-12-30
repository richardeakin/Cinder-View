
# view sources
list( APPEND VIEW_SOURCES
	${VIEW_SOURCE_PATH}/ui/Button.cpp
	${VIEW_SOURCE_PATH}/ui/Control.cpp
	${VIEW_SOURCE_PATH}/ui/Graph.cpp
	${VIEW_SOURCE_PATH}/ui/Image.cpp
	${VIEW_SOURCE_PATH}/ui/ImageView.cpp
	${VIEW_SOURCE_PATH}/ui/Interface3d.cpp
	${VIEW_SOURCE_PATH}/ui/Label.cpp
	${VIEW_SOURCE_PATH}/ui/Layer.cpp
	${VIEW_SOURCE_PATH}/ui/Renderer.cpp
	${VIEW_SOURCE_PATH}/ui/ScrollView.cpp
	${VIEW_SOURCE_PATH}/ui/Selector.cpp
	${VIEW_SOURCE_PATH}/ui/Slider.cpp
	${VIEW_SOURCE_PATH}/ui/Suite.cpp
	${VIEW_SOURCE_PATH}/ui/TextManager.cpp
	${VIEW_SOURCE_PATH}/ui/View.cpp
)

# cppformat
list( APPEND VIEW_SOURCES
	${VIEW_SOURCE_PATH}/cppformat/format.cc
)

add_library( cinder-view ${VIEW_SOURCES} )
