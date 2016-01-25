#pragma once

#include "ui/Renderer.h"

#include <memory>

namespace ui {

typedef std::shared_ptr<class Filter>   FilterRef;

class Filter {
  public:
	virtual void process( Renderer *ren, const FrameBufferRef &frameBuffer ) = 0;
};

} // namespace ui
