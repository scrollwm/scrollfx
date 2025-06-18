#include <stdlib.h>
#include "color.h"

static const struct wlr_color_primaries COLOR_PRIMARIES_SRGB = { // code point 1
	.red = { 0.640, 0.330 },
	.green = { 0.300, 0.600 },
	.blue = { 0.150, 0.060 },
	.white = { 0.3127, 0.3290 },
};

static const struct wlr_color_primaries COLOR_PRIMARIES_BT2020 = { // code point 9
	.red = { 0.708, 0.292 },
	.green = { 0.170, 0.797 },
	.blue = { 0.131, 0.046 },
	.white = { 0.3127, 0.3290 },
};

void sway_color_primaries_from_named(struct wlr_color_primaries *out,
		enum wlr_color_named_primaries named) {
	switch (named) {
	case WLR_COLOR_NAMED_PRIMARIES_SRGB:
		*out = COLOR_PRIMARIES_SRGB;
		return;
	case WLR_COLOR_NAMED_PRIMARIES_BT2020:
		*out = COLOR_PRIMARIES_BT2020;
		return;
	}
	abort();
}
