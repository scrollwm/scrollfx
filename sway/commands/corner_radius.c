#include <stdlib.h>
#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/arrange.h"
#include "sway/tree/container.h"
#include "sway/tree/root.h"
#include "log.h"

static void arrange_corner_radius_iter(struct sway_container *con, void *data) {
	con->corner_radius = config->corner_radius;
}

bool cmd_corner_radius_parse_value(char *arg, int* result) {
	char *inv;
	int value = strtol(arg, &inv, 10);
	if (strlen(inv) > 0 || value < 0) {
		return false;
	}
	*result = value;
	return true;
}

struct cmd_results *cmd_corner_radius(int argc, char **argv) {
	struct cmd_results *error = checkarg(argc, "corner_radius", EXPECTED_AT_LEAST, 1);
	if (error) {
		return error;
	}

	int value;
	if (!cmd_corner_radius_parse_value(argv[0], &value)) {
		return cmd_results_new(CMD_FAILURE, "corner_radius expected a non-negative integer");
	}

	config->corner_radius = value;

	if (!config->handler_context.container) {
		root_for_each_container(arrange_corner_radius_iter, NULL);
		arrange_root();
	}

	// Adjust titlebar padding for corners
	if (value > config->titlebar_h_padding) {
		config->titlebar_h_padding = value;
	}
	if (value > (int)container_titlebar_height()) {
		config->titlebar_v_padding = (value - config->font_height) / 2;
	}

	return cmd_results_new(CMD_SUCCESS, NULL);
}
