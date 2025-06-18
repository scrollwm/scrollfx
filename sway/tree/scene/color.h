#ifndef _SWAY_SCENE_COLOR_H
#define _SWAY_SCENE_COLOR_H

#include <wlr/render/color.h>

void sway_color_primaries_from_named(struct wlr_color_primaries *out,
		enum wlr_color_named_primaries named);

#endif // _SWAY_SCENE_COLOR_H
