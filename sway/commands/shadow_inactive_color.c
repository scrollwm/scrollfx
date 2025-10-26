#include <string.h>
#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/arrange.h"
#include "util.h"

struct cmd_results *cmd_shadow_inactive_color(int argc, char **argv) {
	struct cmd_results *error = checkarg(argc, "shadow_inactive_color", EXPECTED_AT_LEAST, 1);

	if (error) {
		return error;
	}

	uint32_t color;
	if (!parse_color(argv[0], &color)) {
		return cmd_results_new(CMD_INVALID, "Invalid %s color %s",
				"shadow_inactive_color", argv[0]);
	}
	color_to_rgba(config->shadow_inactive_color, color);

	arrange_root();
	return cmd_results_new(CMD_SUCCESS, NULL);
}
