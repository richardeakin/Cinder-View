
# view sources
list( APPEND VIEW_SOURCES
	${VIEW_SOURCE_PATH}/view/Button.cpp
	${VIEW_SOURCE_PATH}/view/Control.cpp
	${VIEW_SOURCE_PATH}/view/ImageView.cpp
	${VIEW_SOURCE_PATH}/view/Interface3d.cpp
	${VIEW_SOURCE_PATH}/view/Label.cpp
#	${VIEW_SOURCE_PATH}/view/Layer.cpp
	${VIEW_SOURCE_PATH}/view/ScrollView.cpp
	${VIEW_SOURCE_PATH}/view/Selector.cpp
	${VIEW_SOURCE_PATH}/view/Slider.cpp
	${VIEW_SOURCE_PATH}/view/Suite.cpp
	${VIEW_SOURCE_PATH}/view/TextManager.cpp
	${VIEW_SOURCE_PATH}/view/View.cpp
)

# cppformat
list( APPEND VIEW_SOURCES
	${VIEW_SOURCE_PATH}/cppformat/format.cc
)
