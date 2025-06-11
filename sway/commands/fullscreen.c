#include <strings.h>
#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/arrange.h"
#include "sway/tree/container.h"
#include "sway/tree/view.h"
#include "util.h"

struct cmd_results *cmd_fullscreen_movefocus(int argc, char **argv) {
	struct cmd_results *error;
	if ((error = checkarg(argc, "fullscreen_movefocus", EXPECTED_AT_LEAST, 1))) {
		return error;
	}
	config->fullscreen_movefocus = parse_boolean(argv[0], config->fullscreen_movefocus);
	return cmd_results_new(CMD_SUCCESS, NULL);
}

// fullscreen [enable|disable|toggle] [global]
struct cmd_results *cmd_fullscreen(int argc, char **argv) {
	struct cmd_results *error = NULL;
	if ((error = checkarg(argc, "fullscreen", EXPECTED_AT_MOST, 2))) {
		return error;
	}
	if (!root->outputs->length) {
		return cmd_results_new(CMD_FAILURE,
				"Can't run this command while there are no outputs connected.");
	}
	struct sway_container *container = config->handler_context.container;

	if (!container) {
		// If the focus is not a container, do nothing successfully
		return cmd_results_new(CMD_SUCCESS, NULL);
	} else if (!container->pending.workspace) {
		// If in the scratchpad, operate on the highest container
		while (container->pending.parent) {
			container = container->pending.parent;
		}
	}

	bool is_fullscreen = container->pending.fullscreen_mode != FULLSCREEN_NONE;
	bool global = false;
	bool enable = !is_fullscreen;

	if (argc >= 1) {
		if (strcasecmp(argv[0], "global") == 0) {
			global = true;
		} else {
			enable = parse_boolean(argv[0], is_fullscreen);
		}
	}

	if (argc >= 2) {
		global = strcasecmp(argv[1], "global") == 0;
	}

	enum sway_fullscreen_mode mode = FULLSCREEN_NONE;
	if (enable) {
		mode = global ? FULLSCREEN_GLOBAL : FULLSCREEN_WORKSPACE;
	}

	container_set_fullscreen(container, mode);
	arrange_root();

	return cmd_results_new(CMD_SUCCESS, NULL);
}

// fullscreen_application [enable|disable|toggle|reset]
struct cmd_results *cmd_fullscreen_application(int argc, char **argv) {
	struct cmd_results *error = NULL;
	if ((error = checkarg(argc, "fullscreen_application", EXPECTED_AT_MOST, 1))) {
		return error;
	}
	if (!root->outputs->length) {
		return cmd_results_new(CMD_FAILURE,
				"Can't run this command while there are no outputs connected.");
	}
	struct sway_container *container = config->handler_context.container;

	if (!container) {
		// If the focus is not a container, do nothing successfully
		return cmd_results_new(CMD_SUCCESS, NULL);
	} else if (!container->pending.workspace) {
		// If in the scratchpad, operate on the highest container
		while (container->pending.parent) {
			container = container->pending.parent;
		}
	}

	enum sway_fullscreen_app_mode mode = container->pending.fullscreen_application;

	bool is_fullscreen = false;
	if (mode == FULLSCREEN_APP_ENABLED ||
		(mode == FULLSCREEN_APP_DEFAULT && container->pending.fullscreen_mode != FULLSCREEN_NONE)) {
		is_fullscreen = true;
	}
	bool enable = !is_fullscreen;
	mode = enable ? FULLSCREEN_APP_ENABLED : FULLSCREEN_APP_DISABLED;

	if (argc >= 1) {
		if (strcasecmp(argv[0], "reset") == 0) {
			mode = FULLSCREEN_APP_DEFAULT;
		} else {
			enable = parse_boolean(argv[0], is_fullscreen);
			mode = enable ? FULLSCREEN_APP_ENABLED : FULLSCREEN_APP_DISABLED;
		}
	}

	container_set_fullscreen_application(container, mode);

	return cmd_results_new(CMD_SUCCESS, NULL);
}

