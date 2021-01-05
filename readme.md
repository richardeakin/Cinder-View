#### Cinder-View

UI toolkit for Cinder C++ framework

Cinder-View is a UI toolkit for the Cinder C++ framework, developed out of the needs of real-world production applications largely within the interactive installation realm. At the core, it is a 2d scene graph that is intended to be manipulated by multiple simultaneous users, many times on massively large canvases. The toolkit aims to focus on a high quality render for user interfaces (ex. pre-multiplied alpha compositing), inspired by Apple’s UIView although meant to be used in cross-platform, large display scale scenarios.

The core principle that lead the design of this toolkit was to ease the development of modern user interfaces similar to what you find on the web or mobile platforms like iOS (things like layout, animation, transitions), while at the same time giving space for ustom graphics techniques or special hardware needs. See the shortlist of projects using Cinder-View towards the end of this post for concrete examples.

Cinder-View is easily integrated into cinder projects as a [cinderblock](https://libcinder.org/docs/guides/cinder-blocks/index.html)
using [Tinderbox](https://libcinder.org/docs/guides/tinderbox/index.html), or by using the provided cmake configuration.

### Toolkit Features

__Multi-touch__ - direct support for multi touch / multi user scenarios (the original reason this library was created)

__Library of Controls (‘widgets’)__ - Button, ImageView, HSlider, VSlider, Label, Checkbox, Numberbox, TextField, all standard stuff for ui toolkits. 

__ScrollViews__ - building blocks for scrollable content, both free moving and paging. ScrollViews can be nested, if parent ScrollViews give an opportunity for the touch to be ‘stolen’.

__Compositing__ - adjusting a View’s alpha will (by default) cause it to be rendered to a framebuffer, then composited with its parent. This can be specified as either standard or pre-multiplied alpha blending.

__Filters__ - can be added to a View to provide post effects. Includes basic blur and dropshadow filters.

__Layout__ - each View can use a Layout object to set the bounds of it’s children based on some simple ruleset. Currently includes: BoxLayout, LinearLayout, VerticalLayout, HorizontalLayout

__Responder Chain__ - facilitates tex entry by tabbing between widgets.

__Gestures__ - higher level touch heuristics, includes (multi) tap and swipe gesture trackers.
