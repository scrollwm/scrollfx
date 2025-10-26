#include <stdlib.h>
#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/container.h"
#include "log.h"

struct cmd_results *cmd_dim_inactive(int argc, char **argv) {
	struct cmd_results *error = checkarg(argc, "dim_inactive", EXPECTED_AT_LEAST, 1);

	if (error) {
		return error;
	}

	struct sway_container *container = config->handler_context.container;
	if (!container) {
		return cmd_results_new(CMD_FAILURE, "dim_inactive can only be used with for_window");
	}

	char *err;
	float val = strtof(argv[0], &err);
	if (*err || val < 0.0f || val > 1.0f) {
		return cmd_results_new(CMD_FAILURE,
				"dim_inactive float invalid. Must be between 0.0 and 1.0");
	}

	container->dim = val;

	return cmd_results_new(CMD_SUCCESS, NULL);
}
