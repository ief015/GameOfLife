# Changelog
**Current version: 0.3.1**


### 0.3.1 (Aug 17 2019)
**Fixes/Changes**
- Changed initial zoom level (x1 -> x8)
- Outline given to intro/controls text


### 0.3.0 (Aug 17 2019)
**New Features**
- Added grid while zoomed in (togglable with G key, grid shows at x4+ zoom)

**Fixes/Changes**
- Increased zoom-in limit (x10 -> x100)
- Camera movement now properly accounts for delta-time (panning/zooming was too quick on very high framerates)


### 0.2.1 (Aug 17 2019)
**Fixes/Changes**
- Placing cells with negative coordinates now behaves correctly.
- Included "cursor" (mouse x/y world coords) in debug.


### 0.2.0 (Aug 06 2019)
**New Features**
- Added randomize ruleset option in ruleset menu.
- Added settings menu.
- Added ability to change target fps.
- Added ability to change step rate. (left/right [ ] brackets)
- Updated controls list text.
- This changelog!

**Fixes/Changes**
- Update/render timings in debug mode are now more accurate.
- Separated simulation steps/second from framerate. (Ability to run more than one step per frame)
- Sleeping now more accurately accounts for delta time. (Resulting in much less frame skips)
- Debug "fps" now "frames/sec".
- Included "steps/sec" in debug.


### 0.1.0 (Aug 01 2019)
- Initial release!